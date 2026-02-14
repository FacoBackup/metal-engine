#include "LogService.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <sstream>

namespace Metal {

    ApplicationContext* LogService::currentContext = nullptr;

    LogService::LogService(ApplicationContext &context) : AbstractRuntimeComponent(context) {
        currentContext = &context;
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

    const char* LogService::getLevelAnsiColor(LogLevel level) {
        switch (level) {
            case LogLevel::Trace: return "\033[90m"; // Gray
            case LogLevel::Debug: return "\033[37m"; // White
            case LogLevel::Info:  return "\033[97m"; // Bright White
            case LogLevel::Warn:  return "\033[93m"; // Bright Yellow
            case LogLevel::Error: return "\033[91m"; // Bright Red
            case LogLevel::Fatal: return "\033[41m\033[97m"; // Red Background, White Text
            default:              return "\033[0m";
        }
    }

}
