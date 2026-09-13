#pragma once

#include <string>
#include <map>
#include <memory>
#include <chrono>
#include <vector>
#include <nlohmann/json.hpp>

namespace fungal::production {

using json = nlohmann::json;

struct MetricPoint {
    std::chrono::system_clock::time_point timestamp;
    double value;
};

class Metrics {
public:
    static Metrics& instance();

    // Record metrics
    void increment_counter(const std::string& name, double amount = 1.0);
    void set_gauge(const std::string& name, double value);
    void record_histogram(const std::string& name, double value);
    void record_duration_ms(const std::string& name, double duration_ms);

    // Get metric values
    double get_counter(const std::string& name) const;
    double get_gauge(const std::string& name) const;
    json get_histogram_stats(const std::string& name) const;

    // Node metrics
    void record_node_decision(const std::string& node_id, bool success);
    void record_node_energy(const std::string& node_id, double energy);
    void record_node_processing_time(const std::string& node_id, double ms);

    // System metrics
    void record_signal_broadcast(const std::string& node_id);
    void record_signal_received(const std::string& node_id);
    void record_constraint_violation(const std::string& constraint_type);

    // Get aggregated metrics
    json get_node_metrics(const std::string& node_id) const;
    json get_system_metrics() const;
    json get_all_metrics() const;

    // Export metrics in Prometheus format
    std::string export_prometheus() const;

    // Export metrics in JSON format
    json export_json() const;

    // Reset metrics
    void reset_all();
    void reset_metric(const std::string& name);

    // Metrics history
    std::vector<MetricPoint> get_metric_history(const std::string& name) const;
    void set_retention_period(std::chrono::hours period);

private:
    Metrics() = default;

    std::map<std::string, double> counters_;
    std::map<std::string, double> gauges_;
    std::map<std::string, std::vector<double>> histograms_;
    std::map<std::string, std::vector<MetricPoint>> metric_history_;
    std::chrono::hours retention_period_ = std::chrono::hours(24);

    void prune_old_history();
};

}  // namespace fungal::production
