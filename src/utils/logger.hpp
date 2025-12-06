#pragma once

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <memory>
#include <string>

namespace bolt {
namespace utils {

// ============================================================================
// Logging Wrapper
// ============================================================================

class Logger {
public:
    // Singleton access
    static Logger& instance();

    // Logging methods
    template <typename... Args>
    void trace(Args&&... args) {
        logger_->trace(std::forward<Args>(args)...);
    }

    template <typename... Args>
    void debug(Args&&... args) {
        logger_->debug(std::forward<Args>(args)...);
    }

    template <typename... Args>
    void info(Args&&... args) {
        logger_->info(std::forward<Args>(args)...);
    }

    template <typename... Args>
    void warn(Args&&... args) {
        logger_->warn(std::forward<Args>(args)...);
    }

    template <typename... Args>
    void error(Args&&... args) {
        logger_->error(std::forward<Args>(args)...);
    }

    template <typename... Args>
    void critical(Args&&... args) {
        logger_->critical(std::forward<Args>(args)...);
    }

    // Configuration
    void setLevel(spdlog::level::level_enum level);
    void setPattern(const std::string& pattern);

private:
    Logger();
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::shared_ptr<spdlog::logger> logger_;
};

// Convenience macros
#define BOLT_TRACE(...) bolt::utils::Logger::instance().trace(__VA_ARGS__)
#define BOLT_DEBUG(...) bolt::utils::Logger::instance().debug(__VA_ARGS__)
#define BOLT_INFO(...) bolt::utils::Logger::instance().info(__VA_ARGS__)
#define BOLT_WARN(...) bolt::utils::Logger::instance().warn(__VA_ARGS__)
#define BOLT_ERROR(...) bolt::utils::Logger::instance().error(__VA_ARGS__)
#define BOLT_CRITICAL(...) bolt::utils::Logger::instance().critical(__VA_ARGS__)

}  // namespace utils
}  // namespace bolt
