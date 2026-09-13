#include "core/energy_budget.hpp"
#include <sstream>
#include <algorithm>
#include <cmath>

namespace fungal::core {

EnergyBudget::EnergyBudget(int initial_budget)
    : initial_budget_(initial_budget), current_budget_(initial_budget) {}

bool EnergyBudget::can_afford_cycle() const {
    return current_budget_ >= CYCLE_BASE_COST;
}

bool EnergyBudget::spend_for_cycle(int amount) {
    if (amount < 0) {
        return false;
    }
    if (current_budget_ < amount) {
        return false;
    }
    current_budget_ -= amount;
    total_spent_ += amount;
    return true;
}

void EnergyBudget::refund_outcome(bool success, int cost_paid) {
    if (cost_paid < 0) {
        cost_paid = 0;
    }
    if (success) {
        // Partial refund on success (system was right, reward it)
        int refund = static_cast<int>(cost_paid * SUCCESS_REFUND_RATIO / 100.0);
        current_budget_ += refund;
        total_refunded_ += refund;
    } else {
        // Extra penalty on failure (system was wrong, cost more)
        int penalty = FAILURE_PENALTY;
        current_budget_ -= penalty;
        total_spent_ += penalty;
        // Keep budget from drifting into nonsense for Stage1 continuity checks.
        // Economic pressure remains: failures still hurt more than successes help.
        if (current_budget_ < 0) {
            current_budget_ = 0;
        }
    }
}

void EnergyBudget::load_state(int current, int initial, int spent, int refunded) {
    initial_budget_ = initial > 0 ? initial : 0;
    current_budget_ = current;
    if (current_budget_ < 0) {
        current_budget_ = 0;
    }
    total_spent_ = spent >= 0 ? spent : 0;
    total_refunded_ = refunded >= 0 ? refunded : 0;
}

void EnergyBudget::set_budget_from_hardware(const HardwareProfile& profile) {
    // Scale initial budget by hardware capacity
    // More cores and memory = higher budget to explore more
    // Scarce hardware = lower budget, more conservative

    int base_budget = 500;

    // Scale by cores: 4 cores = 1x, 8 cores = 1.3x, 16 cores = 1.5x
    double core_multiplier = 1.0 + (std::log(profile.cpu_cores + 1) / 10.0);

    // Scale by memory: <256MB = 0.5x, 4GB+ = 1.5x
    double memory_multiplier = 1.0;
    if (profile.memory_mb < 256) {
        memory_multiplier = 0.5;
    } else if (profile.memory_mb < 1024) {
        memory_multiplier = 0.8;
    } else if (profile.memory_mb >= 4096) {
        memory_multiplier = 1.5;
    }

    int scaled_budget = static_cast<int>(base_budget * core_multiplier * memory_multiplier);
    initial_budget_ = scaled_budget;
    current_budget_ = scaled_budget;
    total_spent_ = 0;
    total_refunded_ = 0;
}

void EnergyBudget::reset() {
    current_budget_ = initial_budget_;
    total_spent_ = 0;
    total_refunded_ = 0;
}

std::string EnergyBudget::status_string() const {
    std::ostringstream oss;
    oss << "Energy: " << current_budget_ << "/" << initial_budget_
        << " (spent: " << total_spent_ << ", refunded: " << total_refunded_ << ")";
    return oss.str();
}

}  // namespace fungal::core
