#pragma once

#include "core/hardware_aware_scheduler.hpp"
#include <string>

namespace fungal::core {

// Energy budget: real scarcity constraint
// Cycles consume energy, success refunds less than failure costs
// When budget exhausted, system must stop or reduce work

class EnergyBudget {
public:
    explicit EnergyBudget(int initial_budget = 1000);

    // Can we afford to run a cycle? (check before executing)
    bool can_afford_cycle() const;

    // Spend energy for a cycle (called before strategy execution)
    // Returns true if budget allows; false if would go negative
    bool spend_for_cycle(int amount);

    // Refund after observing outcome
    // success: true if outcome matched prediction
    // cost_paid: how much was spent for this cycle
    void refund_outcome(bool success, int cost_paid);

    // Current budget level
    int current_budget() const { return current_budget_; }
    int initial_budget() const { return initial_budget_; }
    int total_spent() const { return total_spent_; }
    int total_refunded() const { return total_refunded_; }

    // Restore durable counters without fake spend/refund side effects
    void load_state(int current, int initial, int spent, int refunded);

    // Set budget from hardware profile
    void set_budget_from_hardware(const HardwareProfile& profile);

    // Reset budget to initial value
    void reset();

    // Get status string for logging
    std::string status_string() const;

    static constexpr int cycle_base_cost() { return CYCLE_BASE_COST; }

private:
    int initial_budget_;
    int current_budget_;
    int total_spent_ = 0;
    int total_refunded_ = 0;

    // Energy costs
    static constexpr int CYCLE_BASE_COST = 10;
    static constexpr int SUCCESS_REFUND_RATIO = 40;  // refund 40% of cost on success
    static constexpr int FAILURE_PENALTY = 30;       // extra cost on failure
};

}  // namespace fungal::core
