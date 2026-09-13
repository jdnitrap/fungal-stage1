#include "fungal_system.hpp"
#include <iostream>
#include <string>

int main() {
    using namespace fungal;

    std::cout << "=== Fungal System - Modular Architecture ===" << std::endl;

    FungalSystem system;
    system.initialize();

    std::string sample_code = R"(
    def recursive_function(n):
        if n <= 1:
            return 1
        return n * recursive_function(n - 1)

    def iterative_function(n):
        result = 1
        for i in range(n):
            result *= (i + 1)
        return result
    )";

    std::cout << "\nAnalyzing code..." << std::endl;
    system.analyze_code(sample_code);

    std::cout << "Running reasoning cycle..." << std::endl;
    system.run_reasoning_cycle(12);

    CodeStats stats;
    stats.max_nesting_depth = 5;
    stats.branch_count = 2;
    stats.function_count = 2;
    stats.token_count = 50;
    system.apply_measurement_feedback(stats);

    std::cout << "\nSystem Status:" << std::endl;
    std::cout << "  Cycles completed: " << system.get_cycle_count() << std::endl;
    std::cout << "  Active nodes: " << system.get_active_node_count() << std::endl;
    std::cout << "  Average claim confidence: " << system.get_average_claim_confidence() << std::endl;

    auto verified_claims = system.get_verified_claims();
    std::cout << "  Verified claims: " << verified_claims.size() << std::endl;

    for (const auto& claim : verified_claims) {
        std::cout << "    - " << claim.statement << " (confidence: " << claim.confidence << ")" << std::endl;
    }

    std::cout << "\n=== System Complete ===" << std::endl;

    return 0;
}
