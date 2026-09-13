#include "core/test_oracle.hpp"
#include <algorithm>
#include <cstring>

namespace fungal::core {

TestOracle::TestOracle() {
    // Add curated test cases for v1
    // Each represents a real bug pattern or correct code

    // Pattern 1: NULL pointer dereference
    add_test_case("int* p = nullptr; int x = *p;", true);
    add_test_case("int* p = nullptr; if (p) { int x = *p; }", false);

    // Pattern 2: Uninitialized variable
    add_test_case("int x; int y = x + 5;", true);
    add_test_case("int x = 0; int y = x + 5;", false);

    // Pattern 3: Buffer overflow
    add_test_case("char buf[10]; strcpy(buf, very_long_string);", true);
    add_test_case("char buf[10]; strncpy(buf, string, 9);", false);

    // Pattern 4: Off-by-one in loop
    add_test_case("int arr[10]; for (int i = 0; i <= 10; i++) arr[i] = 0;", true);
    add_test_case("int arr[10]; for (int i = 0; i < 10; i++) arr[i] = 0;", false);

    // Pattern 5: Use after free
    add_test_case("int* p = new int(5); delete p; int x = *p;", true);
    add_test_case("int* p = new int(5); int x = *p; delete p;", false);

    // Pattern 6: Memory leak
    add_test_case("void f() { int* p = new int(5); return; }", true);
    add_test_case("void f() { int* p = new int(5); delete p; return; }", false);

    // Pattern 7: Resource leak
    add_test_case("void f() { FILE* f = fopen(\"file.txt\", \"r\"); return; }", true);
    add_test_case("void f() { FILE* f = fopen(\"file.txt\", \"r\"); fclose(f); return; }", false);

    // Pattern 8: Race condition (simplified)
    add_test_case("int x; thread t1([&] { x = 1; }); thread t2([&] { x = 2; });", true);
    add_test_case("int x; mutex m; thread t1([&] { lock_guard<mutex> l(m); x = 1; }); "
                  "thread t2([&] { lock_guard<mutex> l(m); x = 2; });", false);

    // Pattern 9: Integer overflow
    add_test_case("int x = INT_MAX; int y = x + 1;", true);
    add_test_case("long x = INT_MAX; long y = x + 1;", false);

    // Pattern 10: Division by zero
    add_test_case("int x = 10; int y = 0; int z = x / y;", true);
    add_test_case("int x = 10; int y = 5; int z = x / y;", false);
}

void TestOracle::add_test_case(const std::string& code, bool has_bug) {
    test_cases_[code] = has_bug;
}

bool TestOracle::has_bug(const std::string& code_snippet) {
    // Exact match first
    auto it = test_cases_.find(code_snippet);
    if (it != test_cases_.end()) {
        return it->second;
    }

    // Fuzzy match: find similar test case
    // (handles variations in formatting/whitespace)
    for (const auto& [test_code, bug] : test_cases_) {
        if (fuzzy_match(code_snippet, test_code)) {
            return bug;
        }
    }

    // Default: assume no bug if not recognized
    // This is conservative: we prefer false negatives to false positives
    return false;
}

bool TestOracle::fuzzy_match(const std::string& code, const std::string& test_code) const {
    // Simple fuzzy matching: check if code contains key patterns from test
    // Remove common variations (whitespace, comments)

    // Quick check: if code is very different in length, probably not a match
    if (code.length() < test_code.length() / 2 ||
        code.length() > test_code.length() * 2) {
        return false;
    }

    // Check if most of the test pattern appears in the code
    // (handles extra whitespace, minor variations)
    size_t test_pos = 0;
    size_t code_pos = 0;

    while (test_pos < test_code.length() && code_pos < code.length()) {
        if (test_code[test_pos] == ' ' || test_code[test_pos] == '\n' || test_code[test_pos] == '\t') {
            test_pos++;
            continue;
        }
        if (code[code_pos] == ' ' || code[code_pos] == '\n' || code[code_pos] == '\t') {
            code_pos++;
            continue;
        }

        if (test_code[test_pos] != code[code_pos]) {
            return false;
        }

        test_pos++;
        code_pos++;
    }

    // If we matched most of the test pattern, consider it a match
    return test_pos > (test_code.length() * 0.7);
}

}  // namespace fungal::core
