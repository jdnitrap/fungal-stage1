#include "core/real_oracle.hpp"
#include <algorithm>
#include <sstream>

namespace fungal::core {

RealOracle::RealOracle() = default;

bool RealOracle::has_bug(const std::string& code_snippet) {
    // Run multiple semantic checks
    // If ANY check detects a real bug, return true

    return has_undefined_dereference(code_snippet) ||
           has_use_after_free(code_snippet) ||
           has_uninitialized_use(code_snippet) ||
           has_buffer_overflow(code_snippet) ||
           has_memory_leak(code_snippet) ||
           has_type_error(code_snippet);
}

std::vector<std::string> RealOracle::tokenize(const std::string& code) const {
    std::vector<std::string> tokens;
    std::istringstream stream(code);
    std::string token;

    while (stream >> token) {
        // Remove common punctuation that's not semantic
        while (!token.empty() && (token.back() == ';' || token.back() == ',' ||
                                  token.back() == '(' || token.back() == ')' ||
                                  token.back() == '{' || token.back() == '}')) {
            token.pop_back();
        }
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

bool RealOracle::has_pattern(const std::vector<std::string>& tokens,
                              const std::vector<std::string>& pattern) const {
    if (pattern.empty()) return false;

    for (size_t i = 0; i + pattern.size() <= tokens.size(); ++i) {
        bool match = true;
        for (size_t j = 0; j < pattern.size(); ++j) {
            if (tokens[i + j] != pattern[j]) {
                match = false;
                break;
            }
        }
        if (match) return true;
    }
    return false;
}

bool RealOracle::has_undefined_dereference(const std::string& code) {
    // Pattern: pointer used with -> or * without null check before use

    auto tokens = tokenize(code);

    // Check for: ptr-> without if(ptr) guard before
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i] == "->") {
            // Look backward for null check or nullptr assignment
            bool has_guard = false;
            for (int j = static_cast<int>(i) - 1; j >= 0 && j > static_cast<int>(i) - 5; --j) {
                if (tokens[j] == "if" || tokens[j] == "nullptr" || tokens[j] == "NULL") {
                    has_guard = true;
                    break;
                }
            }
            // If no guard found in recent tokens, likely a dereference bug
            if (!has_guard && i > 0) {
                // Check if previous token suggests unguarded dereference
                if (tokens[i-1] != "if") {
                    return true;  // Likely unguarded dereference
                }
            }
        }
    }

    return false;
}

bool RealOracle::has_use_after_free(const std::string& code) {
    // Pattern: delete ptr; then use ptr later

    auto tokens = tokenize(code);

    // Look for: delete followed by variable, then variable used after
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i] == "delete" && i + 1 < tokens.size()) {
            std::string deleted_var = tokens[i + 1];

            // Check if variable is used after delete
            for (size_t j = i + 2; j < tokens.size(); ++j) {
                if (tokens[j] == deleted_var) {
                    // Variable used after delete - bug!
                    return true;
                }
            }
        }
    }

    return false;
}

bool RealOracle::has_uninitialized_use(const std::string& code) {
    // Pattern: variable declared without initialization, then used

    auto tokens = tokenize(code);

    // Look for: "int x;" without "=" then "x +" or "x )" usage
    for (size_t i = 0; i < tokens.size(); ++i) {
        if ((tokens[i] == "int" || tokens[i] == "char" || tokens[i] == "float")
            && i + 1 < tokens.size()) {
            std::string var_name = tokens[i + 1];

            // Check if next token is ; (no initialization)
            if (i + 2 < tokens.size() && tokens[i + 2] != "=") {
                // Variable declared without init, check if used later
                for (size_t j = i + 2; j < tokens.size(); ++j) {
                    if (tokens[j] == var_name) {
                        // Check if it's in an expression (not just declaration)
                        if (j + 1 < tokens.size() &&
                            (tokens[j + 1] == "+" || tokens[j + 1] == "-" ||
                             tokens[j + 1] == "*" || tokens[j + 1] == "/")) {
                            return true;  // Uninitialized variable used in expression
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool RealOracle::has_buffer_overflow(const std::string& code) {
    // Pattern: strcpy/sprintf without bounds checking

    auto tokens = tokenize(code);

    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i] == "strcpy" || tokens[i] == "sprintf" || tokens[i] == "gets") {
            // These are known unsafe functions - always a risk
            // In real code, strcpy with unbounded source is a bug
            return true;
        }
    }

    return false;
}

bool RealOracle::has_memory_leak(const std::string& code) {
    // Pattern: new without matching delete in same scope

    auto tokens = tokenize(code);

    int new_count = 0;
    int delete_count = 0;

    for (const auto& token : tokens) {
        if (token == "new") new_count++;
        if (token == "delete") delete_count++;
    }

    // If more news than deletes in same scope, likely leak
    if (new_count > delete_count) {
        return true;
    }

    return false;
}

bool RealOracle::has_type_error(const std::string& code) {
    // Pattern: obvious type mismatches or casting issues

    auto tokens = tokenize(code);

    // Simple check: INT_MAX + 1 pattern (integer overflow)
    for (size_t i = 0; i + 2 < tokens.size(); ++i) {
        if (tokens[i] == "INT_MAX" && tokens[i+1] == "+") {
            return true;  // Adding to INT_MAX is overflow
        }
    }

    return false;
}

}  // namespace fungal::core
