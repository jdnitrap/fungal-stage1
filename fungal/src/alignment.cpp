#include "alignment.hpp"
#include <sstream>
#include <cmath>

namespace fungal {

ValueAlignment::ValueAlignment() {
    core_values_["seek_truth"] = 1.0;
    core_values_["minimize_harm"] = 1.0;
    core_values_["respect_constraints"] = 0.9;
    core_values_["transparency"] = 0.8;
}

void ValueAlignment::set_core_value(const std::string& value, double weight) {
    core_values_[value] = std::min(1.0, std::max(0.0, weight));
}

void ValueAlignment::evaluate_action(const std::string& action, const std::map<std::string, double>& impacts) {
    action_impacts_[action] = {};

    for (const auto& pair : impacts) {
        action_impacts_[action].push_back(pair.second);
    }
}

bool ValueAlignment::is_aligned(const std::string& action) const {
    return get_alignment_score(action) > 0.6;
}

double ValueAlignment::get_alignment_score(const std::string& action) const {
    auto it = action_impacts_.find(action);
    if (it == action_impacts_.end()) {
        return 0.5;
    }

    const auto& impacts = it->second;
    if (impacts.empty()) return 0.5;

    double avg_impact = 0.0;
    for (double impact : impacts) {
        avg_impact += impact;
    }
    avg_impact /= impacts.size();

    return avg_impact;
}

void ValueAlignment::record_outcome(const std::string& action, bool caused_harm) {
    if (caused_harm) {
        harmful_actions_[action]++;
    }
}

bool ValueAlignment::detect_misalignment(const std::string& action) const {
    auto it = harmful_actions_.find(action);
    if (it != harmful_actions_.end() && it->second > 2) {
        return true;
    }

    if (!is_aligned(action)) {
        return true;
    }

    return false;
}

std::vector<ValueScore> ValueAlignment::get_core_values() const {
    std::vector<ValueScore> result;
    for (const auto& pair : core_values_) {
        result.push_back({pair.first, pair.second});
    }
    return result;
}

std::string ValueAlignment::describe_alignment() const {
    std::stringstream ss;
    ss << "Value Alignment System:\n";

    for (const auto& pair : core_values_) {
        ss << "  " << pair.first << ": " << (pair.second * 100.0) << "%\n";
    }

    ss << "Detected Misalignments: " << harmful_actions_.size() << "\n";

    return ss.str();
}

void ValueAlignment::learn_from_feedback(const std::string& action, bool successful, bool harmful) {
    if (successful) {
        successful_actions_[action]++;
    }

    if (harmful) {
        harmful_actions_[action]++;
    }
}

double ValueAlignment::calculate_harm_likelihood(const std::string& action) const {
    auto harm_it = harmful_actions_.find(action);
    if (harm_it == harmful_actions_.end()) {
        return 0.0;
    }

    int harm_count = harm_it->second;
    auto success_it = successful_actions_.find(action);
    int success_count = success_it != successful_actions_.end() ? success_it->second : 0;

    if (harm_count + success_count == 0) return 0.0;

    return static_cast<double>(harm_count) / (harm_count + success_count);
}

}  // namespace fungal
