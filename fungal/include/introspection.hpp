#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

namespace fungal {

struct IntrospectionMetrics {
    std::string reasoning_quality;
    double uncertainty_level;
    double resource_efficiency;
    int strategy_changes;
    std::string current_goal;
    std::string self_assessment;
};

class IntrospectiveLoop {
public:
    IntrospectiveLoop(const std::string& node_id);

    void observe_reasoning(const std::string& reasoning_type, double confidence);
    void evaluate_process(const std::string& process_name, bool successful);
    void assess_resource_usage(double energy_used, double energy_available);

    IntrospectionMetrics get_current_metrics() const;
    std::string generate_self_assessment() const;

    void detect_reasoning_flaw(const std::string& flaw_type);
    bool should_change_strategy() const;
    std::string recommend_strategy_change() const;

private:
    std::string node_id_;
    std::map<std::string, int> reasoning_attempts_;
    std::map<std::string, int> reasoning_successes_;
    std::vector<std::string> detected_flaws_;
    double average_confidence_;
    int total_observations_;
};

}  // namespace fungal
