#pragma once

#include "core/control_loop.hpp"
#include <string>
#include <vector>

namespace fungal::core {

// REAL ORACLE: Deterministic static heuristics for C++ bug detection
// NOT actual compilation or runtime checking.
//
// Purpose: Provide better-grounded labels than TestOracle by using
// pattern-based token analysis for common real C++ bugs.
// Independent of PatternMatcherStrategy to avoid false convergence.
// Next step: true external oracle (compile/run or real analyzer).

class RealOracle : public Oracle {
public:
    RealOracle();

    // Check if code snippet contains a potential bug via static heuristics
    // Uses: token pattern analysis for undefined deref, UAF, uninitialized,
    //       buffer overflow, memory leak, type error
    // Does NOT compile or execute code
    bool has_bug(const std::string& code_snippet) override;

private:
    // Static heuristic checkers for potential C++ bugs (pattern-based, not compilation)
    bool has_undefined_dereference(const std::string& code);
    bool has_use_after_free(const std::string& code);
    bool has_uninitialized_use(const std::string& code);
    bool has_buffer_overflow(const std::string& code);
    bool has_memory_leak(const std::string& code);
    bool has_type_error(const std::string& code);

    // Helper: parse code structure
    std::vector<std::string> tokenize(const std::string& code) const;
    bool has_pattern(const std::vector<std::string>& tokens,
                     const std::vector<std::string>& pattern) const;
};

}  // namespace fungal::core
