#include "core/strategy.hpp"
#include <algorithm>
#include <cmath>

namespace fungal::core {

PatternMatcherStrategy::PatternMatcherStrategy() = default;

StrategyResult PatternMatcherStrategy::apply(const std::string& code_snippet) {
    int energy_cost = 10;
    int pattern_count = 0;

    if (has_null_dereference_pattern(code_snippet)) {
        pattern_count++;
    }
    if (has_uninitialized_var_pattern(code_snippet)) {
        pattern_count++;
    }
    if (has_off_by_one_pattern(code_snippet)) {
        pattern_count++;
    }

    bool claim = pattern_count > 0;
    double confidence = compute_confidence(pattern_count, static_cast<int>(code_snippet.length()));

    std::string reasoning = claim ?
        "Found " + std::to_string(pattern_count) + " bug pattern(s) in code" :
        "No obvious bug patterns detected";

    return StrategyResult{
        .claim = claim,
        .strategy_confidence = confidence,
        .reasoning = reasoning,
        .energy_cost = energy_cost
    };
}

bool PatternMatcherStrategy::has_null_dereference_pattern(const std::string& code) {
    // Prefer explicit dereference patterns over any '*', which false-positives on multiplication.
    const bool has_arrow = code.find("->") != std::string::npos;
    const bool has_star_deref =
        code.find("*p") != std::string::npos ||
        code.find("* p") != std::string::npos ||
        code.find("*ptr") != std::string::npos ||
        code.find("* ptr") != std::string::npos;

    if (!has_arrow && !has_star_deref) {
        return false;
    }

    // If there is a nearby null/guard check, treat as safer.
    const bool has_guard =
        code.find("if (p)") != std::string::npos ||
        code.find("if(p)") != std::string::npos ||
        code.find("if (ptr)") != std::string::npos ||
        code.find("if(ptr)") != std::string::npos ||
        code.find("nullptr") != std::string::npos && code.find("if") != std::string::npos;

    // nullptr assignment + dereference without guard remains suspicious.
    if (code.find("nullptr") != std::string::npos && (has_arrow || has_star_deref) &&
        code.find("if") == std::string::npos) {
        return true;
    }

    return (has_arrow || has_star_deref) && !has_guard;
}

bool PatternMatcherStrategy::has_uninitialized_var_pattern(const std::string& code) {
    // Look for a simple "type name;" declaration without '=' before first ';'
    // then a later use. This remains heuristic, not a real parser.
    const bool has_typed_decl =
        code.find("int ") != std::string::npos ||
        code.find("char ") != std::string::npos;

    if (!has_typed_decl) {
        return false;
    }

    size_t semi = code.find(';');
    if (semi == std::string::npos || semi == 0) {
        return false;
    }

    bool has_assign_before_first_semi = false;
    for (size_t i = 0; i < semi; ++i) {
        if (code[i] == '=') {
            has_assign_before_first_semi = true;
            break;
        }
    }

    if (has_assign_before_first_semi) {
        return false;
    }

    // Require a second statement that likely uses a variable (another ';' or '=')
    // after the first declaration.
    return code.find(';', semi + 1) != std::string::npos;
}

bool PatternMatcherStrategy::has_off_by_one_pattern(const std::string& code) {
    // Focus on classic inclusive upper-bound loop over an array.
    const bool has_for = code.find("for") != std::string::npos;
    const bool has_inclusive = code.find("<=" ) != std::string::npos;
    const bool has_index =
        code.find("[i]") != std::string::npos ||
        code.find("[i + 1]") != std::string::npos ||
        code.find("[i+1]") != std::string::npos;

    return has_for && has_inclusive && has_index;
}

double PatternMatcherStrategy::compute_confidence(int pattern_count, int code_length) {
    if (pattern_count == 0) {
        return 0.2;
    }

    double base_confidence = std::min(0.9, 0.3 + (pattern_count * 0.25));
    double length_factor = 1.0 / (1.0 + (code_length / 100.0));
    double confidence = base_confidence * (0.7 + 0.3 * length_factor);
    return std::min(0.95, std::max(0.05, confidence));
}

}  // namespace fungal::core
