#pragma once

#include <spdlog/spdlog.h>
#include <filesystem>

#include "RemoveCopyAssignEtc.h"

#include <string>
#include <sstream>

#include "LogBuffer.h"

#include <spdlog/sinks/dist_sink.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <utility>

// ReSharper disable CppClangTidyClangDiagnosticPragmaMessages
#ifndef RESOURCE_FILENAME_ATTRIBUTE
#pragma message( "RESOURCE_FILENAME_ATTRIBUTE was not defined while compiling " __FILE__ ".")
#define RESOURCE_FILENAME_ATTRIBUTE "UnknownFilename-PleaseInclude-VersionInformation.h-Before-Logger.H"
#pragma message( "We defined and set it to " RESOURCE_FILENAME_ATTRIBUTE " now!")
#endif

#ifndef ASSEMBLY_VERSION_ATTRIBUTE
#pragma message( "ASSEMBLY_VERSION_ATTRIBUTE was not defined while compiling " __FILE__ ".")
#define ASSEMBLY_VERSION_ATTRIBUTE "UnknownVersion.H"
#pragma message( "We defined and set it to " ASSEMBLY_VERSION_ATTRIBUTE " now!")
#endif
// ReSharper restore CppClangTidyClangDiagnosticPragmaMessages


namespace ed
{
    namespace model
    {
        class Logger final
        {
        public:
            DISALLOW_COPY_MOVE(Logger);
        private:
            Logger() = default;
        public:
            ~Logger() = default;

            static Logger& Inst();

            void SetPathName(const std::filesystem::path& fileName);
            [[nodiscard]] std::filesystem::path GetPathName() const;
            [[nodiscard]] std::wstring GetDir() const;

            void SetOutputToConsole(bool isOutputToConsole);
            [[nodiscard]] bool IsOutputToConsole() const { return isOutputToConsole_; }

            void SetLogBuffer(std::shared_ptr<LogBuffer> logBuffer);

            std::shared_ptr<spdlog::logger> L(const std::string& delimiterBetweenDateAndTime = " ");
        private:
            std::shared_ptr<spdlog::logger> spdLogger_;

            std::shared_ptr<spdlog::details::thread_pool> threadPool_;
            std::shared_ptr<LogBuffer> spLogBuffer_;
            std::filesystem::path pathName_;
            bool logIsToBeReinitialized_ = true;
			bool isOutputToConsole_ = false;
        };
    }
}


inline ed::model::Logger& ed::model::Logger::Inst()
{
    static Logger logger;
    return logger;
}

inline void ed::model::Logger::SetPathName(const std::filesystem::path& fileName)
{
    pathName_ = fileName;
    logIsToBeReinitialized_ = true;
}

inline std::filesystem::path ed::model::Logger::GetPathName() const
{
	return pathName_;
}

inline std::wstring ed::model::Logger::GetDir() const
{
	const auto* pathNamePtr = pathName_.c_str();
	if
	(
		const wchar_t* found;
		(found = wcsrchr(pathNamePtr, L'\\')) != nullptr ||
		(found = wcsrchr(pathNamePtr, L'/')) != nullptr
	)
	{
		return {pathNamePtr, found};
	}

	return pathName_;
}

inline void ed::model::Logger::SetLogBuffer(std::shared_ptr<LogBuffer> logBuffer)
{
    spLogBuffer_ = std::move(logBuffer);
    logIsToBeReinitialized_ = true;
}

inline void ed::model::Logger::SetOutputToConsole(bool isOutputToConsole)
{
    if (isOutputToConsole_ != isOutputToConsole)
    {
        isOutputToConsole_ = isOutputToConsole;
        logIsToBeReinitialized_ = true;
    }
}


inline std::shared_ptr<spdlog::logger> ed::model::Logger::L(const std::string& delimiterBetweenDateAndTime)
{
    if (logIsToBeReinitialized_)
    {
        auto distributedSink = std::make_shared<spdlog::sinks::dist_sink_st>();
        if (!pathName_.empty())
        {
            const auto rotatingFileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                pathName_.string(), 1024 * 1024, 10);
            distributedSink->add_sink(rotatingFileSink);
        }
        if (spLogBuffer_ != nullptr)
        {
            distributedSink->add_sink(spLogBuffer_);
        }

		if (isOutputToConsole_)
		{
			const auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			distributedSink->add_sink(consoleSink);
		}

        threadPool_ = std::make_shared<spdlog::details::thread_pool>(65536, 2);

        // Create an async_logger using that custom thread pool
        spdLogger_ = std::make_shared<spdlog::async_logger>(
            RESOURCE_FILENAME_ATTRIBUTE,
            distributedSink,
            threadPool_,
            spdlog::async_overflow_policy::block
        );

        spdlog::register_logger(spdLogger_);
        spdlog::set_default_logger(spdLogger_);

        spdlog::set_pattern(std::string("%Y-%m-%d") + delimiterBetweenDateAndTime + "%H:%M:%S.%f %L [%t] %v");
        spdlog::set_level(spdlog::level::debug);
        {
            std::ostringstream oss;
            oss << "Log for " << RESOURCE_FILENAME_ATTRIBUTE << ".exe " << ASSEMBLY_VERSION_ATTRIBUTE << " initialized.";
            spdLogger_->info(oss.str().c_str());
        }
        logIsToBeReinitialized_ = false;
    }
    return spdLogger_;
}


#define SPD_L ed::model::Logger::Inst().L()
#define SPD_LT ed::model::Logger::Inst().L("T")
