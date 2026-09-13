#include "introspection.hpp"
#include <sstream>

namespace fungal {

IntrospectiveLoop::IntrospectiveLoop(const std::string& node_id)
    : node_id_(node_id), average_confidence_(0.0), total_observations_(0) {}

void IntrospectiveLoop::observe_reasoning(const std::string& reasoning_type, double confidence) {
    reasoning_attempts_[reasoning_type]++;
    total_observations_++;

    average_confidence_ = (average_confidence_ * (total_observations_ - 1) + confidence) / total_observations_;
}

void IntrospectiveLoop::evaluate_process(const std::string& process_name, bool successful) {
    if (successful) {
        reasoning_successes_[process_name]++;
    }
}

void IntrospectiveLoop::assess_resource_usage(double energy_used, double energy_available) {
    if (energy_available > 0) {
        double efficiency = 1.0 - (energy_used / energy_available);
        if (efficiency < 0.3) {
            detect_reasoning_flaw("low_resource_efficiency");
        }
    }
}

IntrospectionMetrics IntrospectiveLoop::get_current_metrics() const {
    IntrospectionMetrics metrics;

    if (average_confidence_ > 0.8) {
        metrics.reasoning_quality = "high";
    } else if (average_confidence_ > 0.6) {
        metrics.reasoning_quality = "medium";
    } else {
        metrics.reasoning_quality = "low";
    }

    metrics.uncertainty_level = 1.0 - average_confidence_;
    metrics.strategy_changes = detected_flaws_.size();
    metrics.self_assessment = generate_self_assessment();

    return metrics;
}

std::string IntrospectiveLoop::generate_self_assessment() const {
    std::stringstream ss;
    ss << "Node " << node_id_ << " Assessment: ";
    ss << "Observations=" << total_observations_;
    ss << " AvgConfidence=" << average_confidence_;
    ss << " DetectedFlaws=" << detected_flaws_.size();

    return ss.str();
}

void IntrospectiveLoop::detect_reasoning_flaw(const std::string& flaw_type) {
    detected_flaws_.push_back(flaw_type);
}

bool IntrospectiveLoop::should_change_strategy() const {
    return detected_flaws_.size() > 2 || average_confidence_ < 0.5;
}

std::string IntrospectiveLoop::recommend_strategy_change() const {
    if (detected_flaws_.empty()) {
        return "no_change_needed";
    }

    std::string primary_flaw = detected_flaws_.back();

    if (primary_flaw == "low_confidence") {
        return "increase_verification_redundancy";
    } else if (primary_flaw == "low_resource_efficiency") {
        return "optimize_resource_allocation";
    } else if (primary_flaw == "slow_convergence") {
        return "increase_exploration";
    }

    return "adaptive_strategy";
}

}  // namespace fungal
