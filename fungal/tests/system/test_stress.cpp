#include "../../include/fungal_system.hpp"
#include <gtest/gtest.h>

using namespace fungal;

TEST(StressTest, MultipleCodeAnalyses) {
    FungalSystem system;
    system.initialize();

    for (int i = 0; i < 10; ++i) {
        std::string code = "for (int i = 0; i < " + std::to_string(i) + "; ++i) { x++; }";
        system.analyze_code(code);
    }

    EXPECT_GT(system.get_network().claims.size(), 0);
}

TEST(StressTest, MultiplePhaseCycles) {
    FungalSystem system;
    system.initialize();

    std::string code = "def fib(n): return fib(n-1) + fib(n-2) if n > 1 else 1";
    system.analyze_code(code);

    for (int i = 0; i < 50; ++i) {
        system.run_reasoning_cycle(12);
    }

    EXPECT_EQ(system.get_cycle_count(), 50);
}

TEST(StressTest, HighNodeCount) {
    FungalSystem system;
    system.initialize();

    auto& network = system.get_network();
    for (int i = 0; i < 100; ++i) {
        Claim claim;
        claim.id = "claim_" + std::to_string(i);
        claim.statement = "Test claim " + std::to_string(i);
        claim.confidence = 0.5;
        network.claims.push_back(claim);

        Node node;
        node.id = "node_" + std::to_string(i);
        node.claim_id = claim.id;
        node.energy.available = 1.0;
        network.nodes.push_back(node);
    }

    system.run_reasoning_cycle(12);

    EXPECT_LE(system.get_active_node_count(), 100);
}
