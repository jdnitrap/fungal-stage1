#include "core/control_loop.hpp"
#include "core/external_oracle.hpp"
#include <iostream>
#include <vector>
#include <iomanip>

using namespace fungal::core;

int main() {
    std::cout << "=== Fungal v1 Control Loop with EXTERNAL Oracle ===" << std::endl << std::endl;

    // Create EXTERNAL oracle (real C++ compiler as ground truth)
    // This actually compiles code and checks for real compiler warnings/errors
    auto oracle = std::make_shared<ExternalOracle>();

    // Create strategy
    auto strategy = std::make_shared<PatternMatcherStrategy>();

    // Create control loop
    ControlLoop loop(oracle, strategy);
    loop.initialize_from_hardware();

    HardwareProfile profile = loop.hardware_scheduler().get_current_profile();
    std::cout << "Hardware detected:" << std::endl;
    std::cout << "  CPU cores: " << profile.cpu_cores << std::endl;
    std::cout << "  Memory: " << profile.memory_mb << " MB" << std::endl;
    std::cout << "  Architecture: " << profile.architecture << std::endl << std::endl;

    std::cout << "Oracle: EXTERNAL (real C++ compilation with clang++ -Wall -Wextra -fsanitize=undefined)" << std::endl;
    std::cout << "Strategy: PatternMatcher (weak heuristics for comparison)" << std::endl << std::endl;

    // Test code snippets with REAL bugs
    std::vector<std::string> test_snippets = {
        "int* p = nullptr; int x = *p;",           // REAL BUG: undefined dereference
        "int* p = nullptr; if (p) { int x = *p; }", // OK: guarded
        "int x; int y = x + 5;",                    // REAL BUG: uninitialized use
        "int x = 0; int y = x + 5;",               // OK: initialized
        "char buf[10]; strcpy(buf, very_long_string);", // REAL BUG: strcpy + undefined var
        "char buf[10]; strncpy(buf, string, 9);",  // REAL BUG: undefined string var
        "int* p = new int(5); delete p; int z = *p;", // REAL BUG: use after free
        "int* p = new int(5); int z = *p; delete p;", // OK: use before delete
        "int* p = new int(5);",                    // REAL BUG: memory leak (compiles but bad)
        "int x = INT_MAX; int y = x + 1;",         // REAL BUG: integer overflow
    };

    std::cout << "Running " << test_snippets.size() << " test cycles with EXTERNAL oracle..." << std::endl << std::endl;

    auto results = loop.run_cycles(test_snippets);
    SelfModel& self_model = loop.self_model();

    int real_bugs = 0;
    int strategy_bugs = 0;
    int correct = 0;

    // Per-cycle detailed logging
    std::cout << "=== PER-CYCLE LOGS ===" << std::endl << std::endl;

    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];

        std::string oracle_says = result.oracle_ground_truth ? "BUG" : "OK";
        std::string strategy_says = result.strategy_claim ? "BUG" : "OK";
        std::string match = (result.prediction_correct && result.system_had_energy) ? "CORRECT" : "WRONG";

        std::cout << "Cycle " << (i + 1) << ":" << std::endl;
        std::cout << "  Code: " << test_snippets[i].substr(0, 70);
        if (test_snippets[i].size() > 70) std::cout << "...";
        std::cout << std::endl;
        std::cout << "  Strategy claim: " << strategy_says << std::endl;
        std::cout << "  Oracle label: " << oracle_says << std::endl;
        std::cout << "  Outcome correct: " << (result.prediction_correct ? "true" : "false") << std::endl;
        std::cout << "  Predicted μ (at cycle): " << std::fixed << std::setprecision(3)
                  << result.predicted_success * 100 << "%" << std::endl;
        std::cout << "  Empirical success rate: " << std::fixed << std::setprecision(3)
                  << self_model.get_empirical_success_rate(0) * 100 << "%" << std::endl;
        std::cout << "  Calibration error: " << std::fixed << std::setprecision(3)
                  << self_model.get_calibration_error(0) << std::endl;
        std::cout << "  Energy spent this cycle: " << result.energy_spent << " units" << std::endl;
        std::cout << "  Had energy to run: " << (result.system_had_energy ? "yes" : "no") << std::endl;
        std::cout << "  Result: " << match << std::endl << std::endl;

        if (result.oracle_ground_truth) real_bugs++;
        if (result.strategy_claim) strategy_bugs++;
        if (result.prediction_correct && result.system_had_energy) correct++;
    }

    std::cout << "=== SUMMARY ===" << std::endl << std::endl;

    std::cout << "Results with ExternalOracle:" << std::endl;
    std::cout << "  Total cycles: " << loop.total_cycles_run() << std::endl;
    std::cout << "  Cycles ran: " << loop.cycles_that_ran() << " (had energy)" << std::endl;
    std::cout << "  Correct predictions: " << correct << "/" << loop.cycles_that_ran() << " = "
              << std::fixed << std::setprecision(1)
              << (loop.cycles_that_ran() > 0 ? (100.0 * correct / loop.cycles_that_ran()) : 0)
              << "%" << std::endl;
    std::cout << "  Real bugs detected by oracle: " << real_bugs << std::endl;
    std::cout << "  Bugs claimed by strategy: " << strategy_bugs << std::endl;
    std::cout << "  Final energy budget: " << loop.energy_budget().current_budget() << " units" << std::endl;
    std::cout << "  Final self-model μ: " << std::fixed << std::setprecision(2)
              << self_model.predict_success(0) * 100 << "%" << std::endl;
    std::cout << "  Empirical success rate: " << std::fixed << std::setprecision(2)
              << self_model.get_empirical_success_rate(0) * 100 << "%" << std::endl;
    std::cout << "  Final calibration error: " << std::fixed << std::setprecision(2)
              << self_model.get_calibration_error(0) << std::endl;

    std::cout << std::endl << "Oracle Type: External (real C++ compilation)" << std::endl;
    std::cout << "Compiler: clang++ with -Wall -Wextra -fsanitize=undefined" << std::endl;
    std::cout << "Ground Truth: Actual compiler diagnostics (not pattern matching)" << std::endl;

    return 0;
}
