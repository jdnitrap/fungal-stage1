#include "monitor.hpp"
#include <sstream>
#include <iomanip>

namespace fungal {

Monitor::Monitor(const std::string& node_id)
    : node_id_(node_id), total_decisions_(0), successful_decisions_(0) {}

void Monitor::log_decision(const std::string& action, const std::string& rationale, double energy_used) {
    std::lock_guard<std::mutex> lock(monitor_mutex_);

    AuditEntry entry{
        get_timestamp(),
        node_id_,
        action,
        rationale,
        energy_used,
        false
    };

    audit_trail_.push_back(entry);
    total_decisions_++;
}

void Monitor::log_result(const std::string& action, bool successful) {
    std::lock_guard<std::mutex> lock(monitor_mutex_);

    if (!audit_trail_.empty()) {
        audit_trail_.back().successful = successful;

        if (successful) {
            successful_decisions_++;
        }
    }
}

std::vector<AuditEntry> Monitor::get_audit_trail(int limit) const {
    std::lock_guard<std::mutex> lock(monitor_mutex_);

    std::vector<AuditEntry> result;
    int start = std::max(0, (int)audit_trail_.size() - limit);

    for (int i = start; i < (int)audit_trail_.size(); ++i) {
        result.push_back(audit_trail_[i]);
    }

    return result;
}

void Monitor::clear_old_entries(int age_seconds) {
    std::lock_guard<std::mutex> lock(monitor_mutex_);

    audit_trail_.clear();
}

void Monitor::detect_anomaly(const std::string& metric, double value, double threshold) {
    std::lock_guard<std::mutex> lock(monitor_mutex_);

    if (value > threshold) {
        anomalies_.push_back(metric + " exceeded threshold: " + std::to_string(value));
    }
}

std::vector<std::string> Monitor::get_detected_anomalies() const {
    std::lock_guard<std::mutex> lock(monitor_mutex_);
    return anomalies_;
}

double Monitor::get_average_energy_per_action() const {
    std::lock_guard<std::mutex> lock(monitor_mutex_);

    if (audit_trail_.empty()) return 0.0;

    double total_energy = 0.0;
    for (const auto& entry : audit_trail_) {
        total_energy += entry.energy_consumed;
    }

    return total_energy / audit_trail_.size();
}

int Monitor::get_total_decisions() const {
    return total_decisions_;
}

double Monitor::get_success_rate() const {
    if (total_decisions_ == 0) return 0.0;
    return static_cast<double>(successful_decisions_) / total_decisions_;
}

std::string Monitor::generate_audit_report() const {
    std::lock_guard<std::mutex> lock(monitor_mutex_);

    std::stringstream ss;
    ss << "Audit Report for Node " << node_id_ << "\n";
    ss << "Total Decisions: " << total_decisions_ << "\n";
    ss << "Successful: " << successful_decisions_ << "\n";
    ss << "Success Rate: " << std::fixed << std::setprecision(2)
       << (get_success_rate() * 100.0) << "%\n";
    ss << "Avg Energy per Action: " << get_average_energy_per_action() << "\n";
    ss << "Anomalies Detected: " << anomalies_.size() << "\n";

    return ss.str();
}

std::string Monitor::get_timestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

Killswitch::Killswitch() : armed_(true), triggered_(false), emergency_protocol_("shutdown") {}

void Killswitch::arm() {
    armed_ = true;
}

void Killswitch::disarm() {
    armed_ = false;
}

bool Killswitch::is_armed() const {
    return armed_;
}

void Killswitch::trigger() {
    if (armed_) {
        triggered_ = true;
    }
}

bool Killswitch::was_triggered() const {
    return triggered_;
}

void Killswitch::set_emergency_protocol(const std::string& protocol) {
    emergency_protocol_ = protocol;
}

std::string Killswitch::get_emergency_protocol() const {
    return emergency_protocol_;
}

}  // namespace fungal
