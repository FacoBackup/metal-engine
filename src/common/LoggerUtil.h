#ifndef METAL_ENGINE_LOGSERVICE_H
#define METAL_ENGINE_LOGSERVICE_H

#include <string>
#include <vector>
#include <mutex>

namespace Metal {

    enum class LogLevel {
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Fatal
    };

    struct LogEntry {
        LogLevel level;
        std::string message;
        std::string timestamp;
    };

    namespace LoggerUtil {
        void log(LogLevel level, const std::string &message);

        [[nodiscard]] std::vector<LogEntry> getEntriesSnapshot();
        void clear();

        const char* getLevelName(LogLevel level);
        uint32_t getLevelColor(LogLevel level);
    }

}

#define LOG_TRACE(msg) Metal::LoggerUtil::log(Metal::LogLevel::Trace, msg)
#define LOG_DEBUG(msg) Metal::LoggerUtil::log(Metal::LogLevel::Debug, msg)
#define LOG_INFO(msg)  Metal::LoggerUtil::log(Metal::LogLevel::Info, msg)
#define LOG_WARN(msg)  Metal::LoggerUtil::log(Metal::LogLevel::Warn, msg)
#define LOG_ERROR(msg) Metal::LoggerUtil::log(Metal::LogLevel::Error, msg)
#define LOG_FATAL(msg) Metal::LoggerUtil::log(Metal::LogLevel::Fatal, msg)

#endif
