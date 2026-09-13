#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace fungal {

struct Strategy {
    std::string name;
    std::string description;
    std::vector<std::string> steps;
    double effectiveness;
    int times_used;
};

class StrategyGenerator {
public:
    StrategyGenerator();

    Strategy generate_new_strategy(const std::string& problem_type, const std::vector<std::string>& available_tools);
    void evaluate_strategy(const std::string& strategy_name, bool successful);

    std::vector<Strategy> get_learned_strategies() const;
    std::string get_best_strategy() const;

    void combine_strategies(const std::string& strategy1, const std::string& strategy2, const std::string& new_name);
    Strategy mutate_strategy(const std::string& strategy_name);

    std::string describe_strategy_generation() const;

private:
    std::map<std::string, Strategy> learned_strategies_;
    std::vector<std::string> base_actions_;

    Strategy create_base_strategy(const std::string& problem_type);
};

class MetaReasoning {
public:
    MetaReasoning();

    void evaluate_own_reasoning(const std::string& reasoning_path, bool led_to_success);
    void detect_reasoning_pattern(const std::string& pattern);

    std::string analyze_decision_quality(const std::map<std::string, double>& decision_factors);
    std::string recommend_reasoning_improvement() const;

    void track_reasoning_efficiency(double time_spent, bool successful);
    double get_reasoning_quality() const;

    std::string introspect_on_problem_solving();
    std::vector<std::string> identify_weaknesses() const;

private:
    std::map<std::string, int> reasoning_patterns_;
    std::vector<double> reasoning_quality_history_;
    std::vector<std::string> identified_weaknesses_;
    int total_reasoning_events_;
    int successful_reasoning_events_;
};

class SelfOptimizer {
public:
    SelfOptimizer();

    void run_optimization_cycle();
    void optimize_resource_allocation();
    void improve_decision_making();

    std::string suggest_self_improvement() const;
    bool should_modify_strategy() const;

    void record_optimization_result(const std::string& change, bool improved);

private:
    int optimization_cycles_;
    std::map<std::string, bool> recent_optimizations_;
};

}  // namespace fungal
