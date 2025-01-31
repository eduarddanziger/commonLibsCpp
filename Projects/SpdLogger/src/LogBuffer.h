#pragma once

#include <vector>
#include <mutex>
#include <sstream>

#include "spdlog/sinks/sink.h"

namespace ed
{
    namespace model
    {
        class LogBuffer : public spdlog::sinks::sink
        {
        public:
            LogBuffer() = default;
            std::vector<std::wstring> GetAndClearNextQueueChunk();

            void log(const spdlog::details::log_msg& msg) override;

            void flush() override
            {
            }

        protected:
            void Put(const std::string& val);
        private:
            std::vector<std::wstring> m_queue;
            mutable std::recursive_mutex m_queueGuard;;
        };
    }
}


inline std::vector<std::wstring> ed::model::LogBuffer::GetAndClearNextQueueChunk()
{
    std::vector<std::wstring> result;

    std::lock_guard<std::recursive_mutex> lock(m_queueGuard);

    result.swap(m_queue);
    return result;
}

inline void ed::model::LogBuffer::log(const spdlog::details::log_msg& msg)
{
    std::stringstream ss(msg.formatted.str());
    std::string line;
    while (std::getline(ss, line))
    {
        Put(line);
    }
}

inline void ed::model::LogBuffer::Put(const std::string& val)
{
    std::wstring widenedVal(val.length(), L' ');
    std::copy(val.begin(), val.end(), widenedVal.begin());

    std::lock_guard<std::recursive_mutex> lock(m_queueGuard);
    m_queue.push_back(widenedVal);
}

