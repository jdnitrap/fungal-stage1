#include "core/real_oracle.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <iomanip>

using namespace fungal::core;

// Compile a code snippet and check for compiler warnings/errors
// Returns true if compiler reports issues (likely a bug)
bool check_with_compiler(const std::string& code_snippet, std::string& compiler_output) {
    // Write to temp file with necessary includes
    std::ofstream temp_file("/tmp/test_snippet.cpp");
    temp_file << "#include <iostream>\n";
    temp_file << "#include <cstring>\n";
    temp_file << "#include <climits>\n";
    temp_file << "\n";
    temp_file << "int main() {\n";
    temp_file << "    " << code_snippet << "\n";
    temp_file << "    return 0;\n";
    temp_file << "}\n";
    temp_file.close();

    // Try to compile with clang++ (most verbose warnings)
    // Capture both stdout and stderr
    int result = system("clang++ -Wall -Wextra -fsanitize=undefined /tmp/test_snippet.cpp -o /tmp/test_snippet 2>&1 > /tmp/compile.log 2>&1");

    // Read the compilation output
    std::ifstream log_file("/tmp/compile.log");
    std::stringstream buffer;
    buffer << log_file.rdbuf();
    compiler_output = buffer.str();
    log_file.close();

    // If compilation failed (non-zero exit), there's definitely an issue
    // Also check if there are warnings in the output
    bool has_issues = (result != 0) || compiler_output.find("warning:") != std::string::npos ||
                      compiler_output.find("error:") != std::string::npos;

    return has_issues;
}

int main() {
    std::cout << "=== RealOracle Validator ===" << std::endl;
    std::cout << "Checking RealOracle predictions against real C++ compiler" << std::endl << std::endl;

    // Test code snippets (same as in control_loop_real_demo.cpp)
    std::vector<std::string> test_snippets = {
        "int* p = nullptr; int x = *p;",           // Should have undefined behavior
        "int* p = nullptr; if (p) { int x = *p; }", // Should be OK
        "int x; int y = x + 5;",                    // Should have uninitialized use
        "int x = 0; int y = x + 5;",               // Should be OK
        "char buf[10]; strcpy(buf, very_long_string);", // Should have buffer overflow risk
        "char buf[10]; strncpy(buf, string, 9);",  // Should be OK
        "int* p = new int(5); delete p; int z = *p;", // Should have use-after-free
        "int* p = new int(5); int z = *p; delete p;", // Should be OK
        "int* p = new int(5);",                    // Should have memory leak (but hard to detect at compile time)
        "int x = INT_MAX; int y = x + 1;",         // Should have integer overflow
    };

    std::vector<bool> expected_bugs = {
        true,   // Undefined dereference
        false,  // Guarded
        true,   // Uninitialized
        false,  // Initialized
        true,   // strcpy
        false,  // strncpy (bounds checked)
        true,   // Use after free
        false,  // Use before delete
        true,   // Memory leak
        true,   // Integer overflow
    };

    RealOracle oracle;

    std::cout << std::left
              << std::setw(65) << "Code Snippet"
              << std::setw(15) << "Expected"
              << std::setw(15) << "RealOracle"
              << std::setw(15) << "Compiler"
              << std::setw(10) << "Match" << std::endl;
    std::cout << std::string(120, '-') << std::endl;

    int oracle_correct = 0;
    int oracle_total = 0;

    for (size_t i = 0; i < test_snippets.size(); ++i) {
        std::string compiler_output;
        bool compiler_says_bug = check_with_compiler(test_snippets[i], compiler_output);
        bool oracle_says_bug = oracle.has_bug(test_snippets[i]);
        bool expected_bug = expected_bugs[i];

        std::string oracle_verdict = oracle_says_bug ? "BUG" : "OK";
        std::string compiler_verdict = compiler_says_bug ? "BUG" : "OK";
        std::string expected_verdict = expected_bug ? "BUG" : "OK";

        bool oracle_matches_expected = (oracle_says_bug == expected_bug);
        bool compiler_matches_expected = (compiler_says_bug == expected_bug);

        if (oracle_matches_expected) oracle_correct++;
        oracle_total++;

        std::string match_status = oracle_matches_expected ? "✓" : "✗";

        std::cout << std::left
                  << std::setw(65) << test_snippets[i].substr(0, 64)
                  << std::setw(15) << expected_verdict
                  << std::setw(15) << oracle_verdict
                  << std::setw(15) << compiler_verdict
                  << std::setw(10) << match_status << std::endl;

        if (!compiler_output.empty() && compiler_says_bug) {
            // Show first line of compiler output for context
            std::string first_line = compiler_output.substr(0, compiler_output.find('\n'));
            if (first_line.size() > 100) first_line = first_line.substr(0, 97) + "...";
            std::cout << "  → " << first_line << std::endl;
        }
    }

    std::cout << std::string(120, '-') << std::endl << std::endl;

    std::cout << "Validation Results:" << std::endl;
    std::cout << "  RealOracle accuracy vs expected ground truth: "
              << oracle_correct << "/" << oracle_total << " = "
              << std::fixed << std::setprecision(1)
              << (100.0 * oracle_correct / oracle_total) << "%" << std::endl;
    std::cout << std::endl << "Note: Compiler detection limited to compile-time errors/warnings." << std::endl;
    std::cout << "Runtime bugs (use-after-free, memory leak) may not be caught at compile time." << std::endl;

    return 0;
}
