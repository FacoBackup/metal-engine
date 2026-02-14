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
        static ApplicationContext* currentContext;
        explicit LogService(ApplicationContext &context);

        void log(LogLevel level, const std::string &message) const;

        [[nodiscard]] std::vector<LogEntry> getEntriesSnapshot() const;
        void clear();

        static const char* getLevelName(LogLevel level);
        static uint32_t getLevelColor(LogLevel level);
        static const char* getLevelAnsiColor(LogLevel level);

    private:
        mutable std::vector<LogEntry> entries;
        mutable std::mutex logMutex;
    };

}

#define LOG_TRACE(context, msg) (context).logService.log(Metal::LogLevel::Trace, msg)
#define LOG_DEBUG(context, msg) (context).logService.log(Metal::LogLevel::Debug, msg)
#define LOG_INFO(context, msg)  (context).logService.log(Metal::LogLevel::Info, msg)
#define LOG_WARN(context, msg)  (context).logService.log(Metal::LogLevel::Warn, msg)
#define LOG_ERROR(context, msg) (context).logService.log(Metal::LogLevel::Error, msg)
#define LOG_FATAL(context, msg) (context).logService.log(Metal::LogLevel::Fatal, msg)

#define LOG_TRACE_S(msg) LOG_TRACE(*Metal::LogService::currentContext, msg)
#define LOG_DEBUG_S(msg) LOG_DEBUG(*Metal::LogService::currentContext, msg)
#define LOG_INFO_S(msg)  LOG_INFO(*Metal::LogService::currentContext, msg)
#define LOG_WARN_S(msg)  LOG_WARN(*Metal::LogService::currentContext, msg)
#define LOG_ERROR_S(msg) LOG_ERROR(*Metal::LogService::currentContext, msg)
#define LOG_FATAL_S(msg) LOG_FATAL(*Metal::LogService::currentContext, msg)

#endif
