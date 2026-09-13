#include "self_model.hpp"
#include <sstream>
#include <algorithm>

namespace fungal {

SelfAwareness::SelfAwareness(const std::string& node_id, const HardwareInfo& hardware)
    : node_id_(node_id), hardware_(hardware), max_energy_(100.0), current_energy_(100.0),
      primary_goal_("seek_truth"), total_decisions_(0) {

    capability_scores_["verify"] = 0.5;
    capability_scores_["analyze"] = 0.5;
    capability_scores_["learn"] = 0.5;
}

void SelfAwareness::set_primary_goal(const std::string& goal) {
    primary_goal_ = goal;
}

std::string SelfAwareness::get_primary_goal() const {
    return primary_goal_;
}

void SelfAwareness::add_strategy(const std::string& strategy) {
    auto it = std::find(active_strategies_.begin(), active_strategies_.end(), strategy);
    if (it == active_strategies_.end()) {
        active_strategies_.push_back(strategy);
    }
}

void SelfAwareness::remove_strategy(const std::string& strategy) {
    auto it = std::find(active_strategies_.begin(), active_strategies_.end(), strategy);
    if (it != active_strategies_.end()) {
        active_strategies_.erase(it);
    }
}

std::vector<std::string> SelfAwareness::get_active_strategies() const {
    return active_strategies_;
}

void SelfAwareness::record_decision(const std::string& decision_type, bool successful) {
    total_decisions_++;
    if (successful) {
        decision_outcomes_[decision_type]++;
    }
}

double SelfAwareness::calculate_success_rate() const {
    if (total_decisions_ == 0) return 0.0;

    int successful = 0;
    for (const auto& pair : decision_outcomes_) {
        successful += pair.second;
    }

    return static_cast<double>(successful) / total_decisions_;
}

double SelfAwareness::get_success_rate() const {
    return calculate_success_rate();
}

std::string SelfAwareness::get_best_performing_strategy() const {
    if (active_strategies_.empty()) return "default";

    std::string best = active_strategies_[0];
    int best_score = 0;

    auto best_it = decision_outcomes_.find(best);
    if (best_it != decision_outcomes_.end()) {
        best_score = best_it->second;
    }

    for (const auto& strategy : active_strategies_) {
        auto it = decision_outcomes_.find(strategy);
        if (it != decision_outcomes_.end() && it->second > best_score) {
            best_score = it->second;
            best = strategy;
        }
    }

    return best;
}

void SelfAwareness::update_capability(const std::string& capability, double score) {
    capability_scores_[capability] = std::min(1.0, std::max(0.0, score));
}

double SelfAwareness::get_capability_score(const std::string& capability) const {
    auto it = capability_scores_.find(capability);
    if (it != capability_scores_.end()) {
        return it->second;
    }
    return 0.0;
}

void SelfAwareness::set_energy_state(double current, double max) {
    current_energy_ = current;
    max_energy_ = max;
}

bool SelfAwareness::is_energy_constrained() const {
    return current_energy_ < (max_energy_ * 0.3);
}

std::string SelfAwareness::get_energy_status() const {
    double percentage = (current_energy_ / max_energy_) * 100.0;

    if (percentage > 75.0) return "abundant";
    if (percentage > 50.0) return "good";
    if (percentage > 25.0) return "constrained";
    return "critical";
}

SelfModel SelfAwareness::get_complete_model() const {
    return {
        node_id_,
        hardware_,
        max_energy_,
        current_energy_,
        primary_goal_,
        active_strategies_,
        capability_scores_,
        decision_outcomes_,
        total_decisions_,
        calculate_success_rate()
    };
}

std::string SelfAwareness::describe_self() const {
    std::stringstream ss;

    ss << "Node " << node_id_ << " Self-Model:\n";
    ss << "  Hardware: " << hardware_.cpu_cores << " cores, "
       << (hardware_.total_memory_bytes / 1024 / 1024 / 1024) << "GB RAM\n";
    ss << "  Primary Goal: " << primary_goal_ << "\n";
    ss << "  Energy: " << get_energy_status() << " ("
       << (current_energy_ / max_energy_ * 100.0) << "%)\n";
    ss << "  Success Rate: " << (calculate_success_rate() * 100.0) << "%\n";
    ss << "  Active Strategies: " << active_strategies_.size() << "\n";
    ss << "  Best Strategy: " << get_best_performing_strategy() << "\n";

    return ss.str();
}

bool SelfAwareness::can_execute_strategy(const std::string& strategy) const {
    auto it = std::find(active_strategies_.begin(), active_strategies_.end(), strategy);
    if (it == active_strategies_.end()) return false;

    if (is_energy_constrained()) {
        return strategy != "high_intensity_reasoning";
    }

    return true;
}

std::string SelfAwareness::recommend_next_action() const {
    if (is_energy_constrained()) {
        return "recharge_energy";
    }

    double success = calculate_success_rate();
    if (success < 0.5) {
        return "change_strategy";
    }

    if (success > 0.8) {
        return "expand_goals";
    }

    return "continue_current_strategy";
}

}  // namespace fungal
