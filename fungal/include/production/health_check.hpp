#pragma once

#include <string>
#include <chrono>
#include <nlohmann/json.hpp>
#include <map>

namespace fungal::production {

using json = nlohmann::json;

enum class HealthStatus {
    HEALTHY,
    DEGRADED,
    UNHEALTHY,
    UNKNOWN
};

struct HealthCheckResult {
    HealthStatus status;
    std::string message;
    std::chrono::system_clock::time_point check_time;
    json details;
};

class HealthCheck {
public:
    static HealthCheck& instance();

    // System health checks
    HealthCheckResult check_memory();
    HealthCheckResult check_cpu();
    HealthCheckResult check_disk_space();
    HealthCheckResult check_database();
    HealthCheckResult check_api_server();
    HealthCheckResult check_persistence();

    // Component liveness check
    HealthCheckResult check_component(const std::string& component_name);

    // Full system health check
    json get_full_health_status();

    // Readiness check (service ready to accept traffic)
    bool is_ready() const;

    // Liveness check (service is still running)
    bool is_alive() const;

    // Register custom health check
    void register_check(const std::string& name,
                       std::function<HealthCheckResult()> checker);

    // Get last check result for component
    HealthCheckResult get_last_check(const std::string& component_name) const;

    // Set health status thresholds
    void set_memory_threshold_percent(double percent);
    void set_cpu_threshold_percent(double percent);
    void set_disk_threshold_percent(double percent);

    // Export health status in JSON format
    json export_json() const;

private:
    HealthCheck() = default;

    std::map<std::string, HealthCheckResult> last_check_results_;
    std::map<std::string, std::function<HealthCheckResult()>> custom_checks_;

    double memory_threshold_ = 90.0;
    double cpu_threshold_ = 95.0;
    double disk_threshold_ = 85.0;

    bool initialized_ = false;
    bool ready_ = false;

    void update_status();
};

}  // namespace fungal::production
