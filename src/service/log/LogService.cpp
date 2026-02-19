#include "LogService.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <sstream>

namespace Metal {

    LogService::LogService() : AbstractRuntimeComponent() {
    }

    void LogService::log(LogLevel level, const std::string &message) const {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%H:%M:%S");
        std::string timestamp = ss.str();

        {
            std::lock_guard<std::mutex> lock(logMutex);
            entries.push_back({level, message, timestamp});
        }

        std::cout <<  "[" << timestamp << "] [" << getLevelName(level) << "] " << message << std::endl;
    }

    std::vector<LogEntry> LogService::getEntriesSnapshot() const {
        std::lock_guard<std::mutex> lock(logMutex);
        return entries;
    }

    void LogService::clear() {
        std::lock_guard<std::mutex> lock(logMutex);
        entries.clear();
    }

    const char* LogService::getLevelName(LogLevel level) {
        switch (level) {
            case LogLevel::Trace: return "TRACE";
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info:  return "INFO";
            case LogLevel::Warn:  return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Fatal: return "FATAL";
            default:              return "UNKNOWN";
        }
    }

    uint32_t LogService::getLevelColor(LogLevel level) {
        switch (level) {
            case LogLevel::Trace: return 0xFF888888; // Grey
            case LogLevel::Debug: return 0xFFAAAAAA; // Light Grey
            case LogLevel::Info:  return 0xFFFFFFFF; // White
            case LogLevel::Warn:  return 0xFF00FFFF; // Yellow
            case LogLevel::Error: return 0xFF0000FF; // Red
            case LogLevel::Fatal: return 0xFF000088; // Dark Red
            default:              return 0xFFFFFFFF;
        }
    }

}
