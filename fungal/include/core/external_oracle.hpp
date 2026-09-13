#pragma once

#include "core/control_loop.hpp"
#include <string>
#include <vector>

namespace fungal::core {

// EXTERNAL ORACLE: Real C++ compiler + sanitizers as ground truth
// Compiles code snippets and checks for actual errors/warnings
//
// Purpose: Provide honest ground truth by actually compiling code
// and detecting real bugs via clang++ warnings and UBSan/ASan diagnostics.
// This is the true north-star grounding: not pattern matching, not lookup table,
// but actual semantic analysis via a real C++ compiler.

class ExternalOracle : public Oracle {
public:
    ExternalOracle();

    // Check if code snippet has bugs via real compilation
    // Returns: true if clang++ detects errors/warnings, false if clean
    // Uses: actual C++ compiler with -Wall -Wextra -fsanitize=undefined
    bool has_bug(const std::string& code_snippet) override;

private:
    // Helper: compile code and capture diagnostics
    // Returns: true if compiler reports issues, false if clean compilation
    bool compile_and_check(const std::string& code_snippet, std::string& compiler_output);

    // Helper: wrap raw code in valid C++ with includes
    std::string wrap_in_cpp(const std::string& code_snippet) const;
};

}  // namespace fungal::core
