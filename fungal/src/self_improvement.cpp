#include "self_improvement.hpp"
#include <sstream>
#include <algorithm>
#include <cmath>

namespace fungal {

StrategyGenerator::StrategyGenerator() {
    base_actions_ = {"verify", "analyze", "compare", "integrate", "refine", "validate"};
}

Strategy StrategyGenerator::generate_new_strategy(const std::string& problem_type, const std::vector<std::string>& available_tools) {
    Strategy strategy;
    strategy.name = "adaptive_" + problem_type + "_v" + std::to_string(learned_strategies_.size());
    strategy.description = "Adaptive strategy for " + problem_type;
    strategy.effectiveness = 0.5;
    strategy.times_used = 0;

    for (size_t i = 0; i < std::min(size_t(3), available_tools.size()); ++i) {
        strategy.steps.push_back(available_tools[i]);
    }

    for (const auto& action : base_actions_) {
        if (strategy.steps.size() < 5) {
            strategy.steps.push_back(action);
        }
    }

    learned_strategies_[strategy.name] = strategy;
    return strategy;
}

void StrategyGenerator::evaluate_strategy(const std::string& strategy_name, bool successful) {
    auto it = learned_strategies_.find(strategy_name);
    if (it != learned_strategies_.end()) {
        it->second.times_used++;

        if (successful) {
            it->second.effectiveness = std::min(1.0, it->second.effectiveness + 0.1);
        } else {
            it->second.effectiveness = std::max(0.0, it->second.effectiveness - 0.05);
        }
    }
}

std::vector<Strategy> StrategyGenerator::get_learned_strategies() const {
    std::vector<Strategy> result;
    for (const auto& pair : learned_strategies_) {
        result.push_back(pair.second);
    }

    std::sort(result.begin(), result.end(),
              [](const Strategy& a, const Strategy& b) {
                  return a.effectiveness > b.effectiveness;
              });

    return result;
}

std::string StrategyGenerator::get_best_strategy() const {
    if (learned_strategies_.empty()) return "default";

    std::string best = learned_strategies_.begin()->first;
    double best_effectiveness = learned_strategies_.begin()->second.effectiveness;

    for (const auto& pair : learned_strategies_) {
        if (pair.second.effectiveness > best_effectiveness) {
            best_effectiveness = pair.second.effectiveness;
            best = pair.first;
        }
    }

    return best;
}

void StrategyGenerator::combine_strategies(const std::string& strategy1, const std::string& strategy2, const std::string& new_name) {
    auto it1 = learned_strategies_.find(strategy1);
    auto it2 = learned_strategies_.find(strategy2);

    if (it1 != learned_strategies_.end() && it2 != learned_strategies_.end()) {
        Strategy combined;
        combined.name = new_name;
        combined.description = "Combined strategy from " + strategy1 + " and " + strategy2;

        for (const auto& step : it1->second.steps) {
            combined.steps.push_back(step);
        }
        for (const auto& step : it2->second.steps) {
            combined.steps.push_back(step);
        }

        combined.effectiveness = (it1->second.effectiveness + it2->second.effectiveness) / 2.0;
        combined.times_used = 0;

        learned_strategies_[new_name] = combined;
    }
}

Strategy StrategyGenerator::mutate_strategy(const std::string& strategy_name) {
    auto it = learned_strategies_.find(strategy_name);
    if (it == learned_strategies_.end()) {
        return Strategy{"default", "", {}, 0.5, 0};
    }

    Strategy mutated = it->second;
    mutated.name = strategy_name + "_mutated";

    if (!mutated.steps.empty()) {
        mutated.steps[0] = base_actions_[mutated.steps.size() % base_actions_.size()];
    }

    mutated.effectiveness *= 0.9;
    mutated.times_used = 0;

    learned_strategies_[mutated.name] = mutated;
    return mutated;
}

std::string StrategyGenerator::describe_strategy_generation() const {
    std::stringstream ss;
    ss << "Strategy Generation Status:\n";
    ss << "  Learned Strategies: " << learned_strategies_.size() << "\n";

    auto strategies = get_learned_strategies();
    if (!strategies.empty()) {
        ss << "  Best Strategy: " << strategies[0].name << " (effectiveness: "
           << (strategies[0].effectiveness * 100.0) << "%)\n";
    }

    return ss.str();
}

MetaReasoning::MetaReasoning() : total_reasoning_events_(0), successful_reasoning_events_(0) {}

void MetaReasoning::evaluate_own_reasoning(const std::string& reasoning_path, bool led_to_success) {
    total_reasoning_events_++;

    if (led_to_success) {
        successful_reasoning_events_++;
        reasoning_patterns_[reasoning_path]++;
    }
}

void MetaReasoning::detect_reasoning_pattern(const std::string& pattern) {
    reasoning_patterns_[pattern]++;
}

std::string MetaReasoning::analyze_decision_quality(const std::map<std::string, double>& decision_factors) {
    double sum = 0.0;
    for (const auto& pair : decision_factors) {
        sum += pair.second;
    }

    double average = decision_factors.empty() ? 0.0 : sum / decision_factors.size();

    if (average > 0.8) {
        return "high_quality";
    } else if (average > 0.6) {
        return "good_quality";
    } else if (average > 0.4) {
        return "moderate_quality";
    }

    return "low_quality";
}

std::string MetaReasoning::recommend_reasoning_improvement() const {
    if (get_reasoning_quality() < 0.5) {
        return "increase_verification_depth";
    } else if (get_reasoning_quality() < 0.7) {
        return "add_cross_checking";
    }

    return "maintain_current_approach";
}

void MetaReasoning::track_reasoning_efficiency(double time_spent, bool successful) {
    if (time_spent > 0) {
        reasoning_quality_history_.push_back(successful ? 1.0 / time_spent : 0.5 / time_spent);
    }
}

double MetaReasoning::get_reasoning_quality() const {
    if (total_reasoning_events_ == 0) return 0.0;
    return static_cast<double>(successful_reasoning_events_) / total_reasoning_events_;
}

std::string MetaReasoning::introspect_on_problem_solving() {
    std::stringstream ss;
    ss << "Meta-Reasoning Introspection:\n";
    ss << "  Total Reasoning Events: " << total_reasoning_events_ << "\n";
    ss << "  Successful: " << successful_reasoning_events_ << "\n";
    ss << "  Quality: " << (get_reasoning_quality() * 100.0) << "%\n";
    ss << "  Identified Patterns: " << reasoning_patterns_.size() << "\n";

    return ss.str();
}

std::vector<std::string> MetaReasoning::identify_weaknesses() const {
    return identified_weaknesses_;
}

SelfOptimizer::SelfOptimizer() : optimization_cycles_(0) {}

void SelfOptimizer::run_optimization_cycle() {
    optimization_cycles_++;
    optimize_resource_allocation();
    improve_decision_making();
}

void SelfOptimizer::optimize_resource_allocation() {
}

void SelfOptimizer::improve_decision_making() {
}

std::string SelfOptimizer::suggest_self_improvement() const {
    if (optimization_cycles_ < 5) {
        return "gather_more_data";
    }

    return "evaluate_and_refine";
}

bool SelfOptimizer::should_modify_strategy() const {
    return optimization_cycles_ > 3;
}

void SelfOptimizer::record_optimization_result(const std::string& change, bool improved) {
    recent_optimizations_[change] = improved;
}

}  // namespace fungal
