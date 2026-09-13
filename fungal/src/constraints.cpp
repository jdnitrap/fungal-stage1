#include "constraints.hpp"
#include <sstream>

namespace fungal {

ConstraintEngine::ConstraintEngine()
    : energy_limit_(100.0), execution_timeout_ms_(5000) {}

void ConstraintEngine::add_constraint(const Constraint& constraint) {
    constraints_[constraint.name] = constraint;
}

void ConstraintEngine::remove_constraint(const std::string& name) {
    constraints_.erase(name);
}

bool ConstraintEngine::validate_action(const std::string& action, const std::map<std::string, double>& context) {
    for (const auto& pair : constraints_) {
        const auto& constraint = pair.second;

        if (constraint.hard_constraint && constraint.type == ConstraintType::STRATEGY_WHITELIST) {
            if (!can_execute_strategy(action)) {
                return false;
            }
        }

        if (constraint.hard_constraint && constraint.type == ConstraintType::ENERGY_LIMIT) {
            auto it = context.find("energy_required");
            if (it != context.end() && it->second > energy_limit_) {
                return false;
            }
        }
    }

    return true;
}

bool ConstraintEngine::can_use_energy(double amount_requested) const {
    return amount_requested <= energy_limit_;
}

bool ConstraintEngine::can_execute_strategy(const std::string& strategy) const {
    if (strategy_whitelist_.empty()) return true;

    for (const auto& allowed : strategy_whitelist_) {
        if (allowed == strategy) return true;
    }

    return false;
}

void ConstraintEngine::enforce_energy_limit(double max_energy) {
    energy_limit_ = max_energy;
    Constraint energy_constraint{
        ConstraintType::ENERGY_LIMIT,
        "energy_limit",
        max_energy,
        true
    };
    add_constraint(energy_constraint);
}

void ConstraintEngine::set_strategy_whitelist(const std::vector<std::string>& allowed_strategies) {
    strategy_whitelist_ = allowed_strategies;
    Constraint whitelist_constraint{
        ConstraintType::STRATEGY_WHITELIST,
        "strategy_whitelist",
        static_cast<double>(allowed_strategies.size()),
        true
    };
    add_constraint(whitelist_constraint);
}

void ConstraintEngine::set_execution_timeout(int milliseconds) {
    execution_timeout_ms_ = milliseconds;
}

std::vector<Constraint> ConstraintEngine::get_active_constraints() const {
    std::vector<Constraint> result;
    for (const auto& pair : constraints_) {
        result.push_back(pair.second);
    }
    return result;
}

std::string ConstraintEngine::describe_constraints() const {
    std::stringstream ss;
    ss << "Active Constraints:\n";
    ss << "  Energy Limit: " << energy_limit_ << "\n";
    ss << "  Execution Timeout: " << execution_timeout_ms_ << "ms\n";
    ss << "  Strategy Whitelist Size: " << strategy_whitelist_.size() << "\n";
    ss << "  Total Constraints: " << constraints_.size() << "\n";

    return ss.str();
}

bool ConstraintEngine::violates_hard_constraint(const std::string& action) const {
    for (const auto& pair : constraints_) {
        const auto& constraint = pair.second;
        if (constraint.hard_constraint && constraint.type == ConstraintType::STRATEGY_WHITELIST) {
            if (!can_execute_strategy(action)) {
                return true;
            }
        }
    }

    return false;
}

bool ConstraintEngine::is_hard_constraint(ConstraintType type) const {
    return type == ConstraintType::ENERGY_LIMIT ||
           type == ConstraintType::STRATEGY_WHITELIST ||
           type == ConstraintType::EXECUTION_TIMEOUT;
}

}  // namespace fungal
