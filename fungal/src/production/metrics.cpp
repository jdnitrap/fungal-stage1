#include "production/metrics.hpp"
#include <algorithm>
#include <sstream>

namespace fungal::production {

Metrics& Metrics::instance() {
    static Metrics instance;
    return instance;
}

void Metrics::increment_counter(const std::string& name, double amount) {
    counters_[name] += amount;
}

void Metrics::set_gauge(const std::string& name, double value) {
    gauges_[name] = value;
}

void Metrics::record_histogram(const std::string& name, double value) {
    histograms_[name].push_back(value);
}

void Metrics::record_duration_ms(const std::string& name, double duration_ms) {
    record_histogram(name, duration_ms);
}

double Metrics::get_counter(const std::string& name) const {
    auto it = counters_.find(name);
    return it != counters_.end() ? it->second : 0.0;
}

double Metrics::get_gauge(const std::string& name) const {
    auto it = gauges_.find(name);
    return it != gauges_.end() ? it->second : 0.0;
}

json Metrics::get_histogram_stats(const std::string& name) const {
    json stats;
    auto it = histograms_.find(name);
    if (it == histograms_.end() || it->second.empty()) {
        return stats;
    }

    const auto& data = it->second;
    double sum = 0;
    double min_val = data[0];
    double max_val = data[0];

    for (double val : data) {
        sum += val;
        min_val = std::min(min_val, val);
        max_val = std::max(max_val, val);
    }

    double mean = sum / data.size();
    double variance = 0;
    for (double val : data) {
        variance += (val - mean) * (val - mean);
    }
    variance /= data.size();

    stats["count"] = data.size();
    stats["min"] = min_val;
    stats["max"] = max_val;
    stats["mean"] = mean;
    stats["variance"] = variance;

    return stats;
}

void Metrics::record_node_decision(const std::string& node_id, bool success) {
    std::string counter_name = "node_" + node_id + "_decisions";
    increment_counter(counter_name);
    if (success) {
        std::string success_counter = "node_" + node_id + "_successes";
        increment_counter(success_counter);
    }
}

void Metrics::record_node_energy(const std::string& node_id, double energy) {
    std::string gauge_name = "node_" + node_id + "_energy";
    set_gauge(gauge_name, energy);
}

void Metrics::record_node_processing_time(const std::string& node_id, double ms) {
    std::string hist_name = "node_" + node_id + "_processing_time_ms";
    record_histogram(hist_name, ms);
}

void Metrics::record_signal_broadcast(const std::string& node_id) {
    increment_counter("signals_broadcast");
    increment_counter("node_" + node_id + "_signals_sent");
}

void Metrics::record_signal_received(const std::string& node_id) {
    increment_counter("signals_received");
    increment_counter("node_" + node_id + "_signals_received");
}

void Metrics::record_constraint_violation(const std::string& constraint_type) {
    increment_counter("constraint_violations");
    increment_counter("constraint_violation_" + constraint_type);
}

json Metrics::get_node_metrics(const std::string& node_id) const {
    json metrics;
    std::string prefix = "node_" + node_id;

    for (const auto& [name, value] : counters_) {
        if (name.find(prefix) == 0) {
            metrics[name] = value;
        }
    }

    for (const auto& [name, value] : gauges_) {
        if (name.find(prefix) == 0) {
            metrics[name] = value;
        }
    }

    return metrics;
}

json Metrics::get_system_metrics() const {
    json metrics;
    metrics["counters"] = counters_;
    metrics["gauges"] = gauges_;

    for (const auto& [name, values] : histograms_) {
        if (!values.empty()) {
            metrics["histograms"][name] = get_histogram_stats(name);
        }
    }

    return metrics;
}

json Metrics::get_all_metrics() const {
    return get_system_metrics();
}

std::string Metrics::export_prometheus() const {
    std::stringstream ss;

    for (const auto& [name, value] : counters_) {
        ss << "# TYPE " << name << " counter\n";
        ss << name << " " << value << "\n";
    }

    for (const auto& [name, value] : gauges_) {
        ss << "# TYPE " << name << " gauge\n";
        ss << name << " " << value << "\n";
    }

    return ss.str();
}

json Metrics::export_json() const {
    return get_all_metrics();
}

void Metrics::reset_all() {
    counters_.clear();
    gauges_.clear();
    histograms_.clear();
    metric_history_.clear();
}

void Metrics::reset_metric(const std::string& name) {
    counters_.erase(name);
    gauges_.erase(name);
    histograms_.erase(name);
    metric_history_.erase(name);
}

std::vector<MetricPoint> Metrics::get_metric_history(const std::string& name) const {
    auto it = metric_history_.find(name);
    return it != metric_history_.end() ? it->second : std::vector<MetricPoint>();
}

void Metrics::set_retention_period(std::chrono::hours period) {
    retention_period_ = period;
}

void Metrics::prune_old_history() {
    auto cutoff = std::chrono::system_clock::now() - retention_period_;

    for (auto& [name, history] : metric_history_) {
        auto it = std::remove_if(history.begin(), history.end(),
            [cutoff](const MetricPoint& p) { return p.timestamp < cutoff; });
        history.erase(it, history.end());
    }
}

}  // namespace fungal::production
