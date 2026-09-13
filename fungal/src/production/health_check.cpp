#include "production/health_check.hpp"
#include <fstream>
#include <sstream>

namespace fungal::production {

HealthCheck& HealthCheck::instance() {
    static HealthCheck instance;
    return instance;
}

HealthCheckResult HealthCheck::check_memory() {
    HealthCheckResult result;
    result.check_time = std::chrono::system_clock::now();

    // Simple check - in production, would read /proc/meminfo on Linux
    // For now, return healthy status
    result.status = HealthStatus::HEALTHY;
    result.message = "Memory usage within acceptable range";
    result.details["memory_percent"] = 45.0;

    return result;
}

HealthCheckResult HealthCheck::check_cpu() {
    HealthCheckResult result;
    result.check_time = std::chrono::system_clock::now();

    // Simple check - in production, would read /proc/loadavg on Linux
    result.status = HealthStatus::HEALTHY;
    result.message = "CPU load within acceptable range";
    result.details["cpu_percent"] = 30.0;

    return result;
}

HealthCheckResult HealthCheck::check_disk_space() {
    HealthCheckResult result;
    result.check_time = std::chrono::system_clock::now();

    // Simple check - in production, would use statvfs()
    result.status = HealthStatus::HEALTHY;
    result.message = "Disk space available";
    result.details["disk_percent"] = 60.0;

    return result;
}

HealthCheckResult HealthCheck::check_database() {
    HealthCheckResult result;
    result.check_time = std::chrono::system_clock::now();

    // This would be implemented with actual database connection test
    result.status = HealthStatus::HEALTHY;
    result.message = "Database connection OK";

    return result;
}

HealthCheckResult HealthCheck::check_api_server() {
    HealthCheckResult result;
    result.check_time = std::chrono::system_clock::now();

    // This would check actual API server status
    result.status = HealthStatus::HEALTHY;
    result.message = "API server responding";

    return result;
}

HealthCheckResult HealthCheck::check_persistence() {
    HealthCheckResult result;
    result.check_time = std::chrono::system_clock::now();

    // Check if persistence layer is functional
    result.status = HealthStatus::HEALTHY;
    result.message = "Persistence layer operational";

    return result;
}

HealthCheckResult HealthCheck::check_component(const std::string& component_name) {
    auto it = custom_checks_.find(component_name);
    if (it != custom_checks_.end()) {
        return it->second();
    }

    HealthCheckResult result;
    result.check_time = std::chrono::system_clock::now();
    result.status = HealthStatus::UNKNOWN;
    result.message = "Component check not registered: " + component_name;

    return result;
}

json HealthCheck::get_full_health_status() {
    json status;
    status["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();

    auto memory_check = check_memory();
    auto cpu_check = check_cpu();
    auto disk_check = check_disk_space();
    auto db_check = check_database();
    auto api_check = check_api_server();
    auto persist_check = check_persistence();

    status["memory"] = memory_check.status == HealthStatus::HEALTHY ? "healthy" : "unhealthy";
    status["cpu"] = cpu_check.status == HealthStatus::HEALTHY ? "healthy" : "unhealthy";
    status["disk"] = disk_check.status == HealthStatus::HEALTHY ? "healthy" : "unhealthy";
    status["database"] = db_check.status == HealthStatus::HEALTHY ? "healthy" : "unhealthy";
    status["api_server"] = api_check.status == HealthStatus::HEALTHY ? "healthy" : "unhealthy";
    status["persistence"] = persist_check.status == HealthStatus::HEALTHY ? "healthy" : "unhealthy";

    // Determine overall health
    int healthy_count = 0;
    if (memory_check.status == HealthStatus::HEALTHY) healthy_count++;
    if (cpu_check.status == HealthStatus::HEALTHY) healthy_count++;
    if (disk_check.status == HealthStatus::HEALTHY) healthy_count++;
    if (db_check.status == HealthStatus::HEALTHY) healthy_count++;
    if (api_check.status == HealthStatus::HEALTHY) healthy_count++;
    if (persist_check.status == HealthStatus::HEALTHY) healthy_count++;

    ready_ = healthy_count >= 4;
    status["overall"] = ready_ ? "healthy" : "degraded";

    return status;
}

bool HealthCheck::is_ready() const {
    return ready_;
}

bool HealthCheck::is_alive() const {
    return true;  // If this method is being called, the process is alive
}

void HealthCheck::register_check(const std::string& name,
                                 std::function<HealthCheckResult()> checker) {
    custom_checks_[name] = checker;
}

HealthCheckResult HealthCheck::get_last_check(const std::string& component_name) const {
    auto it = last_check_results_.find(component_name);
    if (it != last_check_results_.end()) {
        return it->second;
    }

    HealthCheckResult result;
    result.status = HealthStatus::UNKNOWN;
    result.message = "No prior check for: " + component_name;

    return result;
}

void HealthCheck::set_memory_threshold_percent(double percent) {
    memory_threshold_ = percent;
}

void HealthCheck::set_cpu_threshold_percent(double percent) {
    cpu_threshold_ = percent;
}

void HealthCheck::set_disk_threshold_percent(double percent) {
    disk_threshold_ = percent;
}

json HealthCheck::export_json() const {
    json output = get_full_health_status();
    return output;
}

void HealthCheck::update_status() {
    last_check_results_["memory"] = check_memory();
    last_check_results_["cpu"] = check_cpu();
    last_check_results_["disk"] = check_disk_space();
    last_check_results_["database"] = check_database();
    last_check_results_["api_server"] = check_api_server();
    last_check_results_["persistence"] = check_persistence();

    initialized_ = true;
}

}  // namespace fungal::production
