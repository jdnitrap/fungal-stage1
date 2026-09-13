#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace fungal {

enum class ConstraintType {
    ENERGY_LIMIT,
    STRATEGY_WHITELIST,
    GOAL_BOUNDARY,
    RESOURCE_LIMIT,
    EXECUTION_TIMEOUT,
    OUTPUT_VALIDATION
};

struct Constraint {
    ConstraintType type;
    std::string name;
    double max_value;
    bool hard_constraint;
};

class ConstraintEngine {
public:
    ConstraintEngine();

    void add_constraint(const Constraint& constraint);
    void remove_constraint(const std::string& name);

    bool validate_action(const std::string& action, const std::map<std::string, double>& context);
    bool can_use_energy(double amount_requested) const;
    bool can_execute_strategy(const std::string& strategy) const;

    void enforce_energy_limit(double max_energy);
    void set_strategy_whitelist(const std::vector<std::string>& allowed_strategies);
    void set_execution_timeout(int milliseconds);

    std::vector<Constraint> get_active_constraints() const;
    std::string describe_constraints() const;

    bool violates_hard_constraint(const std::string& action) const;

private:
    std::map<std::string, Constraint> constraints_;
    double energy_limit_;
    std::vector<std::string> strategy_whitelist_;
    int execution_timeout_ms_;

    bool is_hard_constraint(ConstraintType type) const;
};

}  // namespace fungal
