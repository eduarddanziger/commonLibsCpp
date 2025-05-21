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
            ~Logger();

            static Logger& Inst();

			void Init();
			void Free();

            Logger& SetPathName(const std::filesystem::path& fileName);
            [[nodiscard]] std::filesystem::path GetPathName() const;
            [[nodiscard]] std::wstring GetDir() const;

            Logger& SetOutputToConsole(bool isOutputToConsole);
            [[nodiscard]] bool IsOutputToConsole() const { return isOutputToConsole_; }

            Logger& SetDelimiterBetweenDateAndTime(const std::string& delimiterBetweenDateAndTime = " ");
            [[nodiscard]] std::string GetDelimiterBetweenDateAndTime() const;

            void SetLogBuffer(std::shared_ptr<LogBuffer> logBuffer);

            std::shared_ptr<spdlog::logger> L();
        private:
            std::shared_ptr<LogBuffer> spLogBuffer_;
            std::filesystem::path pathName_;
            bool logIsToBeReinitialized_ = true;
			bool isOutputToConsole_ = false;
            std::string delimiterBetweenDateAndTime_ = " ";
        };
    }
}


inline ed::model::Logger::~Logger()
{
	spdlog::info("Log for {}, version {} finished.", RESOURCE_FILENAME_ATTRIBUTE, ASSEMBLY_VERSION_ATTRIBUTE);
	spdlog::shutdown();
}

inline ed::model::Logger& ed::model::Logger::Inst()
{
    static Logger logger;
    return logger;
}

inline ed::model::Logger& ed::model::Logger::SetPathName(const std::filesystem::path& fileName)
{
    pathName_ = fileName;
    logIsToBeReinitialized_ = true;
	return *this;
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

inline ed::model::Logger& ed::model::Logger::SetDelimiterBetweenDateAndTime(
	const std::string& delimiterBetweenDateAndTime)
{
    if (delimiterBetweenDateAndTime_ != delimiterBetweenDateAndTime)
    {
        delimiterBetweenDateAndTime_ = delimiterBetweenDateAndTime;
        logIsToBeReinitialized_ = true;
	}
	return *this;
}

inline std::string ed::model::Logger::GetDelimiterBetweenDateAndTime() const
{
    return delimiterBetweenDateAndTime_;
}

inline void ed::model::Logger::SetLogBuffer(std::shared_ptr<LogBuffer> logBuffer)
{
    spLogBuffer_ = std::move(logBuffer);
    logIsToBeReinitialized_ = true;
}

inline ed::model::Logger& ed::model::Logger::SetOutputToConsole(bool isOutputToConsole)
{
    if (isOutputToConsole_ != isOutputToConsole)
    {
        isOutputToConsole_ = isOutputToConsole;
        logIsToBeReinitialized_ = true;
    }
	return *this;
}


inline void ed::model::Logger::Init()
{
	if (logIsToBeReinitialized_)
	{
		spdlog::shutdown();

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

			auto threadPool = std::make_shared<
				spdlog::details::thread_pool>(65536, 2);

			// Create an async_logger using that custom thread pool
			const auto spdLogger = std::make_shared<spdlog::async_logger>(
				RESOURCE_FILENAME_ATTRIBUTE,
				distributedSink,
				threadPool,
				spdlog::async_overflow_policy::block
			);

			spdlog::register_logger(spdLogger);
			spdlog::set_default_logger(spdLogger);
		}

		spdlog::set_pattern(std::string("%Y-%m-%d") + delimiterBetweenDateAndTime_ + "%H:%M:%S.%f %L [%t] %v");
		spdlog::set_level(spdlog::level::debug);
		spdlog::info("Log for {}, version {} initialized.", RESOURCE_FILENAME_ATTRIBUTE, ASSEMBLY_VERSION_ATTRIBUTE);
		logIsToBeReinitialized_ = false;
	}
}

inline void ed::model::Logger::Free()
{
	spdlog::info("Log for {}, version {} freed.", RESOURCE_FILENAME_ATTRIBUTE, ASSEMBLY_VERSION_ATTRIBUTE);
	spdlog::shutdown();
	logIsToBeReinitialized_ = true;
}

inline std::shared_ptr<spdlog::logger> ed::model::Logger::L()
{
    Init();
    return spdlog::default_logger();
}


#define SPD_L ed::model::Logger::Inst().L()
#define SPD_LT ed::model::Logger::Inst().L("T")
