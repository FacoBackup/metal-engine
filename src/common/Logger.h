#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <memory>
#include <array>
#include <string>
#include <chrono>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

enum class LogLevel
{
    Info,
    Warning,
    Error
};

struct LogEntry
{
    std::string message;
    LogLevel level;
    std::string sourceFile;
    int sourceLine;
    std::chrono::system_clock::time_point timestamp;
};

class LogStorage
{
public:
    void addLog(const std::string& message, LogLevel level, const std::string& sourceFile, int sourceLine)
    {
        logs[currentIndex] = LogEntry{message, level, sourceFile, sourceLine, std::chrono::system_clock::now()};
        currentIndex = (currentIndex + 1) % MaxLogs;
    }

    const std::array<LogEntry, 100>& getLogs() const { return logs; }

    size_t currentIndex = 0;
private:
    static constexpr size_t MaxLogs = 100;
    std::array<LogEntry, MaxLogs> logs{};
};

namespace Logger
{
    inline std::shared_ptr<spdlog::logger> logger;
    inline std::unique_ptr<LogStorage> logStorage = std::make_unique<LogStorage>();

    inline void Init(const std::string& logFile = "app.log")
    {
        try
        {
            std::size_t queue_size = 8192;

            static auto thread_pool = std::make_shared<spdlog::details::thread_pool>(queue_size, 1);

            auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFile, true);
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

            std::vector<spdlog::sink_ptr> sinks{file_sink, console_sink};

            logger = std::make_shared<spdlog::async_logger>(
                "async_logger", sinks.begin(), sinks.end(),
                thread_pool, spdlog::async_overflow_policy::block
            );

            spdlog::register_logger(logger);
            logger->set_level(spdlog::level::trace);
            logger->flush_on(spdlog::level::warn);
        }
        catch (const spdlog::spdlog_ex& ex)
        {
            std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        }
    }

    // Logging macros
#define LOG(...)    if (Logger::logger) { Logger::logger->info("[{}:{}] {}", __FILE__, __LINE__, fmt::format(__VA_ARGS__)); Logger::logStorage->addLog(fmt::format(__VA_ARGS__), LogLevel::Info, __FILE__, __LINE__); }
#define WARN(...)   if (Logger::logger) { Logger::logger->warn("[{}:{}] {}", __FILE__, __LINE__, fmt::format(__VA_ARGS__)); Logger::logStorage->addLog(fmt::format(__VA_ARGS__), LogLevel::Warning, __FILE__, __LINE__); }
#define ERROR(...)  if (Logger::logger) { Logger::logger->error("[{}:{}] {}", __FILE__, __LINE__, fmt::format(__VA_ARGS__)); Logger::logStorage->addLog(fmt::format(__VA_ARGS__), LogLevel::Error, __FILE__, __LINE__); }
} // namespace Logger

#endif //LOGGER_H
