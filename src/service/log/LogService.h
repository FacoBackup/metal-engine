#ifndef METAL_ENGINE_LOGSERVICE_H
#define METAL_ENGINE_LOGSERVICE_H

#include <string>
#include <vector>
#include <mutex>
#include "../../common/AbstractRuntimeComponent.h"

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

    class LogService : public AbstractRuntimeComponent {
    public:
        explicit LogService();

        void log(LogLevel level, const std::string &message) const;

        [[nodiscard]] std::vector<LogEntry> getEntriesSnapshot() const;
        void clear();

        static const char* getLevelName(LogLevel level);
        static uint32_t getLevelColor(LogLevel level);

    private:
        mutable std::vector<LogEntry> entries;
        mutable std::mutex logMutex;
    };

}

#define LOG_TRACE(msg) ApplicationContext::Get().logService.log(Metal::LogLevel::Trace, msg)
#define LOG_DEBUG(msg) ApplicationContext::Get().logService.log(Metal::LogLevel::Debug, msg)
#define LOG_INFO(msg)  ApplicationContext::Get().logService.log(Metal::LogLevel::Info, msg)
#define LOG_WARN(msg)  ApplicationContext::Get().logService.log(Metal::LogLevel::Warn, msg)
#define LOG_ERROR(msg) ApplicationContext::Get().logService.log(Metal::LogLevel::Error, msg)
#define LOG_FATAL(msg) ApplicationContext::Get().logService.log(Metal::LogLevel::Fatal, msg)

#endif
