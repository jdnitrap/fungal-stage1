#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <mutex>

namespace fungal {

struct AuditEntry {
    std::string timestamp;
    std::string node_id;
    std::string action;
    std::string decision_rationale;
    double energy_consumed;
    bool successful;
};

class Monitor {
public:
    Monitor(const std::string& node_id);

    void log_decision(const std::string& action, const std::string& rationale, double energy_used);
    void log_result(const std::string& action, bool successful);

    std::vector<AuditEntry> get_audit_trail(int limit = 100) const;
    void clear_old_entries(int age_seconds);

    void detect_anomaly(const std::string& metric, double value, double threshold);
    std::vector<std::string> get_detected_anomalies() const;

    double get_average_energy_per_action() const;
    int get_total_decisions() const;
    double get_success_rate() const;

    std::string generate_audit_report() const;

private:
    std::string node_id_;
    std::vector<AuditEntry> audit_trail_;
    std::vector<std::string> anomalies_;
    int total_decisions_;
    int successful_decisions_;
    mutable std::mutex monitor_mutex_;

    std::string get_timestamp() const;
};

class Killswitch {
public:
    Killswitch();

    void arm();
    void disarm();
    bool is_armed() const;
    void trigger();
    bool was_triggered() const;

    void set_emergency_protocol(const std::string& protocol);
    std::string get_emergency_protocol() const;

private:
    bool armed_;
    bool triggered_;
    std::string emergency_protocol_;
};

}  // namespace fungal
