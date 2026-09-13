#include "production/logger.hpp"
#include <spdlog/fmt/ostr.h>

namespace fungal::production {

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

void Logger::init_logger(const std::string& name, const std::string& log_dir) {
    try {
        setup_sinks(log_dir);

        std::vector<spdlog::sink_ptr> sinks;
        // In production, would add actual file and console sinks

        logger_ = std::make_shared<spdlog::logger>(name);
        logger_->set_level(spdlog::level::info);
    } catch (const spdlog::spdlog_ex& ex) {
        throw std::runtime_error("Failed to initialize logger: " + std::string(ex.what()));
    }
}

void Logger::trace(const std::string& message) {
    if (logger_) logger_->trace(message);
}

void Logger::debug(const std::string& message) {
    if (logger_) logger_->debug(message);
}

void Logger::info(const std::string& message) {
    if (logger_) logger_->info(message);
}

void Logger::warn(const std::string& message) {
    if (logger_) logger_->warn(message);
}

void Logger::error(const std::string& message) {
    if (logger_) logger_->error(message);
}

void Logger::critical(const std::string& message) {
    if (logger_) logger_->critical(message);
}

void Logger::log_decision(const std::string& node_id, const std::string& decision, bool success) {
    std::string msg = "Node " + node_id + " decision: " + decision +
                      (success ? " [SUCCESS]" : " [FAILED]");
    info(msg);
}

void Logger::log_error(const std::string& component, const std::string& error_msg) {
    std::string msg = "Component " + component + " error: " + error_msg;
    error(msg);
}

void Logger::log_performance(const std::string& operation, double duration_ms) {
    std::string msg = "Operation " + operation + " took " + std::to_string(duration_ms) + "ms";
    debug(msg);
}

void Logger::log_metric(const std::string& metric_name, double value) {
    std::string msg = "Metric " + metric_name + " = " + std::to_string(value);
    debug(msg);
}

std::shared_ptr<spdlog::logger> Logger::get_logger() const {
    return logger_;
}

void Logger::set_level(spdlog::level::level_enum level) {
    if (logger_) {
        logger_->set_level(level);
    }
}

void Logger::flush() {
    if (logger_) {
        logger_->flush();
    }
}

void Logger::setup_sinks(const std::string& log_dir) {
    // In production, would create rotating file sink and stdout sink
    // auto max_size = 1024 * 1024 * 10;  // 10MB
    // auto max_files = 3;
    // auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
    //     log_dir + "/fungal.log", max_size, max_files);

    // auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
}

}  // namespace fungal::production
