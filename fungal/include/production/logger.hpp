#pragma once

#include <string>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace fungal::production {

class Logger {
public:
    static Logger& instance();

    // Initialize logger
    void init_logger(const std::string& name, const std::string& log_dir = "logs");

    // Log levels
    void trace(const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
    void critical(const std::string& message);

    // Structured logging
    void log_decision(const std::string& node_id, const std::string& decision, bool success);
    void log_error(const std::string& component, const std::string& error_msg);
    void log_performance(const std::string& operation, double duration_ms);
    void log_metric(const std::string& metric_name, double value);

    // Get underlying spdlog logger
    std::shared_ptr<spdlog::logger> get_logger() const;

    // Set log level
    void set_level(spdlog::level::level_enum level);

    // Flush logs
    void flush();

private:
    Logger() = default;
    std::shared_ptr<spdlog::logger> logger_;

    void setup_sinks(const std::string& log_dir);
};

}  // namespace fungal::production
