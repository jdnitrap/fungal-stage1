#pragma once

#include <string>
#include <memory>

namespace fungal::core {

// Result of applying a strategy to a code snippet
struct StrategyResult {
    bool claim;                  // what did the strategy claim? (bug/no bug)
    double strategy_confidence;  // INTERNAL ONLY: strategy-local confidence; self-model owns all probabilities
    std::string reasoning;       // why did it make this claim?
    int energy_cost;             // energy consumed by this strategy
};

// Base class for all strategies
class Strategy {
public:
    virtual ~Strategy() = default;

    // Apply strategy to code snippet
    // Returns claim + confidence + cost
    virtual StrategyResult apply(const std::string& code_snippet) = 0;

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

    StrategyResult apply(const std::string& code_snippet) override;
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
    double compute_confidence(int pattern_count, int code_length);
};

}  // namespace fungal::core
