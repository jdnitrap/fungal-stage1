#include "core/control_loop.hpp"
#include "core/test_oracle.hpp"
#include <iostream>
#include <vector>
#include <iomanip>

using namespace fungal::core;

int main() {
    std::cout << "=== Fungal v1 Control Loop Demo ===" << std::endl << std::endl;

    // Create oracle (ground truth provider)
    auto oracle = std::make_shared<TestOracle>();

    // Create strategy
    auto strategy = std::make_shared<PatternMatcherStrategy>();

    // Create control loop
    ControlLoop loop(oracle, strategy);

    // Initialize from hardware
    loop.initialize_from_hardware();

    HardwareProfile profile = loop.hardware_scheduler().get_current_profile();
    std::cout << "Hardware detected:" << std::endl;
    std::cout << "  CPU cores: " << profile.cpu_cores << std::endl;
    std::cout << "  Memory: " << profile.memory_mb << " MB" << std::endl;
    std::cout << "  OS: " << profile.os_name << std::endl;
    std::cout << "  Arch: " << profile.architecture << std::endl << std::endl;

    std::cout << "Initial state:" << std::endl;
    std::cout << "  Energy: " << loop.energy_budget().current_budget() << " units" << std::endl;
    std::cout << "  Predicted success: " << std::fixed << std::setprecision(2)
              << loop.self_model().predict_success(0) * 100 << "%" << std::endl << std::endl;

    // Test code snippets
    std::vector<std::string> test_snippets = {
        "int* p = nullptr; int x = *p;",           // has bug
        "int* p = nullptr; if (p) { int x = *p; }", // no bug
        "int x; int y = x + 5;",                    // has bug
        "int x = 0; int y = x + 5;",               // no bug
        "char buf[10]; strcpy(buf, very_long_string);", // has bug
        "char buf[10]; strncpy(buf, string, 9);",  // no bug
        "int arr[10]; for (int i = 0; i <= 10; i++) arr[i] = 0;", // has bug
        "int arr[10]; for (int i = 0; i < 10; i++) arr[i] = 0;",  // no bug
    };

    std::cout << "Running " << test_snippets.size() << " test cycles..." << std::endl << std::endl;

    std::cout << std::left << std::setw(50) << "Code Snippet"
              << std::setw(15) << "Oracle Says"
              << std::setw(15) << "System Says"
              << std::setw(12) << "Correct"
              << std::setw(10) << "Budget" << std::endl;
    std::cout << std::string(102, '-') << std::endl;

    auto results = loop.run_cycles(test_snippets);

    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];

        std::string oracle_says = result.oracle_ground_truth ? "BUG" : "OK";
        std::string system_says = result.strategy_claim ? "BUG" : "OK";
        std::string correct = result.prediction_correct && result.system_had_energy ? "✓" : "✗";

        std::cout << std::left << std::setw(50) << test_snippets[i].substr(0, 49)
                  << std::setw(15) << oracle_says
                  << std::setw(15) << system_says
                  << std::setw(12) << correct
                  << std::setw(10) << loop.energy_budget().current_budget() << std::endl;
    }

    std::cout << std::string(102, '-') << std::endl << std::endl;

    // Final statistics
    std::cout << "Final Statistics:" << std::endl;
    std::cout << "  Total cycles: " << loop.total_cycles_run() << std::endl;
    std::cout << "  Cycles that ran: " << loop.cycles_that_ran() << " (had energy)" << std::endl;
    std::cout << "  Correct predictions: " << loop.successful_predictions() << "/"
              << loop.cycles_that_ran() << " = "
              << std::fixed << std::setprecision(1)
              << (loop.cycles_that_ran() > 0 ?
                  (100.0 * loop.successful_predictions() / loop.cycles_that_ran()) : 0)
              << "%" << std::endl;
    std::cout << "  Final energy budget: " << loop.energy_budget().current_budget() << " units" << std::endl;

    SelfModel& self_model = loop.self_model();
    std::cout << "  Self-model prediction: " << std::fixed << std::setprecision(2)
              << self_model.predict_success(0) * 100 << "%" << std::endl;
    std::cout << "  Empirical success rate: " << std::fixed << std::setprecision(2)
              << self_model.get_empirical_success_rate(0) * 100 << "%" << std::endl;
    std::cout << "  Calibration error: " << std::fixed << std::setprecision(2)
              << self_model.get_calibration_error(0) << std::endl;

    return 0;
}
