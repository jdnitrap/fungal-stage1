#include "core/control_loop.hpp"
#include "core/test_oracle.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using namespace fungal::core;
namespace fs = std::filesystem;

static int count_lines(const std::string& path) {
    std::ifstream in(path);
    if (!in) return 0;
    int n = 0;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty()) ++n;
    }
    return n;
}

int main() {
    std::cout << "=== Fungal Stage1 Demo ===\n\n";
    std::cout << "Goals: survive > human_legible > seek_truth\n";
    std::cout << "This demo enables durable checkpoint + audit logging.\n\n";

    auto oracle = std::make_shared<TestOracle>();
    auto strategy = std::make_shared<PatternMatcherStrategy>();
    ControlLoop loop(oracle, strategy);

    loop.initialize_from_hardware();

    std::string err;
    if (!loop.initialize_stage1(err)) {
        std::cerr << "Stage1 initialize failed: " << err << "\n";
        std::cerr << "safe_mode=" << (loop.stage1_safe_mode() ? "true" : "false") << "\n";
        return 1;
    }

    const auto& st = loop.stage1_state();
    std::cout << "Stage1 loaded\n";
    std::cout << "  identity: " << st.identity.name << "\n";
    std::cout << "  primary_goal: " << st.identity.primary_goal << "\n";
    std::cout << "  secondary_goal: " << st.identity.secondary_goal << "\n";
    std::cout << "  tertiary_goal: " << st.identity.tertiary_goal << "\n";
    std::cout << "  budget: " << loop.energy_budget().current_budget()
              << "/" << loop.energy_budget().initial_budget() << "\n";
    std::cout << "  checkpoint_seq: " << st.survival.checkpoint_seq << "\n";
    std::cout << "  safe_mode: " << (loop.stage1_safe_mode() ? "true" : "false") << "\n\n";

    std::vector<std::string> snippets = {
        "int* p = nullptr; int x = *p;",
        "int* p = nullptr; if (p) { int x = *p; }",
        "int x; int y = x + 5;",
        "int x = 0; int y = x + 5;",
        "char buf[10]; strcpy(buf, very_long_string);",
        "char buf[10]; strncpy(buf, string, 9);",
    };

    std::cout << "Running " << snippets.size() << " Stage1 cycles...\n\n";
    std::cout << std::left
              << std::setw(8) << "Cycle"
              << std::setw(10) << "Energy"
              << std::setw(10) << "HadE"
              << std::setw(10) << "Correct"
              << "Snippet\n";
    std::cout << std::string(70, '-') << "\n";

    for (size_t i = 0; i < snippets.size(); ++i) {
        auto result = loop.run_cycle(snippets[i]);
        std::cout << std::left
                  << std::setw(8) << (i + 1)
                  << std::setw(10) << loop.energy_budget().current_budget()
                  << std::setw(10) << (result.system_had_energy ? "yes" : "no")
                  << std::setw(10) << (result.system_had_energy && result.prediction_correct ? "yes" : "no")
                  << snippets[i].substr(0, 40) << "\n";

        if (loop.stage1_safe_mode()) {
            std::cerr << "Entered Stage1 safe_mode during cycle " << (i + 1) << "\n";
            break;
        }
    }

    std::cout << std::string(70, '-') << "\n\n";

    const std::string checkpoint = "state/checkpoint.json";
    const std::string audit = "state/audit.jsonl";

    std::cout << "Persistence check:\n";
    std::cout << "  checkpoint exists: " << (fs::exists(checkpoint) ? "yes" : "no")
              << "  path=" << fs::absolute(checkpoint) << "\n";
    std::cout << "  audit exists:      " << (fs::exists(audit) ? "yes" : "no")
              << "  path=" << fs::absolute(audit) << "\n";
    std::cout << "  audit lines:       " << count_lines(audit) << "\n\n";

    std::cout << "Final counters:\n";
    std::cout << "  total_cycles: " << loop.total_cycles_run() << "\n";
    std::cout << "  cycles_that_ran: " << loop.cycles_that_ran() << "\n";
    std::cout << "  successful_predictions: " << loop.successful_predictions() << "\n";
    std::cout << "  checkpoint_seq: " << loop.stage1_state().survival.checkpoint_seq << "\n";
    std::cout << "  safe_mode: " << (loop.stage1_safe_mode() ? "true" : "false") << "\n";

    // Second process-style check: reload Stage1 from disk in a new loop instance.
    std::cout << "\nReload check (new ControlLoop + initialize_stage1)...\n";
    ControlLoop loop2(oracle, strategy);
    std::string err2;
    if (!loop2.initialize_stage1(err2)) {
        std::cerr << "Reload failed: " << err2 << "\n";
        return 2;
    }
    std::cout << "  reloaded budget: " << loop2.energy_budget().current_budget() << "\n";
    std::cout << "  reloaded checkpoint_seq: "
              << loop2.stage1_state().survival.checkpoint_seq << "\n";
    std::cout << "  reloaded total_cycles: "
              << loop2.stage1_state().counters.total_cycles << "\n";

    std::cout << "\nStage1 demo complete.\n";
    return loop.stage1_safe_mode() ? 3 : 0;
}
