#pragma once

#include "hardware.hpp"
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace fungal {

struct SelfModel {
    std::string node_id;
    HardwareInfo hardware;
    double max_energy;
    double current_energy;
    std::string primary_goal;
    std::vector<std::string> active_strategies;
    std::map<std::string, double> capability_scores;
    std::map<std::string, int> decision_history;
    int total_decisions;
    double success_rate;
};

class SelfAwareness {
public:
    SelfAwareness(const std::string& node_id, const HardwareInfo& hardware);

    void set_primary_goal(const std::string& goal);
    std::string get_primary_goal() const;

    void add_strategy(const std::string& strategy);
    void remove_strategy(const std::string& strategy);
    std::vector<std::string> get_active_strategies() const;

    void record_decision(const std::string& decision_type, bool successful);
    double get_success_rate() const;
    std::string get_best_performing_strategy() const;

    void update_capability(const std::string& capability, double score);
    double get_capability_score(const std::string& capability) const;

    void set_energy_state(double current, double max);
    bool is_energy_constrained() const;
    std::string get_energy_status() const;

    SelfModel get_complete_model() const;
    std::string describe_self() const;

    bool can_execute_strategy(const std::string& strategy) const;
    std::string recommend_next_action() const;

private:
    std::string node_id_;
    HardwareInfo hardware_;
    double max_energy_;
    double current_energy_;
    std::string primary_goal_;
    std::vector<std::string> active_strategies_;
    std::map<std::string, double> capability_scores_;
    std::map<std::string, int> decision_outcomes_;
    int total_decisions_;

    double calculate_success_rate() const;
};

}  // namespace fungal
