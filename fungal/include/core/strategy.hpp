#pragma once

#include "core/self_model.hpp"
#include <string>
#include <memory>
#include <vector>

namespace fungal::core {

// Per-pattern-type task IDs used to key SelfModel accuracy tracking for
// each of PatternMatcherStrategy's real detectors. Task ID 0 is reserved
// for ControlLoop's own overall per-cycle success prediction (unrelated
// to any specific pattern) — these start at 1 so the two never collide.
constexpr int kNullDerefTaskId = 1;
constexpr int kUninitializedVarTaskId = 2;
constexpr int kOffByOneTaskId = 3;

// Result of applying a strategy to a code snippet
struct StrategyResult {
    bool claim;                  // what did the strategy claim? (bug/no bug)
    double strategy_confidence;  // INTERNAL ONLY: strategy-local confidence; self-model owns all probabilities
    std::string reasoning;       // why did it make this claim?
    int energy_cost;             // energy consumed by this strategy
    std::vector<int> triggered_patterns;  // which pattern-type task IDs fired this cycle (see above)
};

// Base class for all strategies
class Strategy {
public:
    virtual ~Strategy() = default;

    // Apply strategy to code snippet, using self_model's current per-pattern-
    // type accuracy (see StrategyResult::triggered_patterns) to weight
    // confidence — a pattern type with a stronger confirmed track record
    // contributes more to the final confidence than one that's been
    // unreliable so far. Untested pattern types default to neutral (0.5)
    // weight (SelfModel::get_accuracy's own default for an unseen task ID).
    // Returns claim + confidence + cost.
    virtual StrategyResult apply(const std::string& code_snippet, const SelfModel& self_model) = 0;

    // Strategy name for logging/debugging
    virtual std::string name() const = 0;

    // Strategy description
    virtual std::string description() const = 0;

    // Reset any internal state (e.g., cache)
    virtual void reset() {}
};

// PatternMatcher strategy: uses code analysis to find patterns
// Bridges to existing tokenizer/analyzer from layer1
class PatternMatcherStrategy : public Strategy {
public:
    PatternMatcherStrategy();

    StrategyResult apply(const std::string& code_snippet, const SelfModel& self_model) override;
    std::string name() const override { return "PatternMatcher"; }
    std::string description() const override {
        return "Analyzes code structure and patterns to detect bugs";
    }

private:
    // Extract methods from layer1 code (tokenizer, analyzer)
    // For now: simple heuristic-based pattern matching

    bool has_null_dereference_pattern(const std::string& code);
    bool has_uninitialized_var_pattern(const std::string& code);
    bool has_off_by_one_pattern(const std::string& code);
    double compute_confidence(const std::vector<int>& triggered_patterns,
                               const SelfModel& self_model,
                               int code_length);
};

}  // namespace fungal::core
