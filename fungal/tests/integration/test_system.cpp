#include "../../include/fungal_system.hpp"
#include <gtest/gtest.h>

using namespace fungal;

TEST(SystemIntegrationTest, InitializeSystem) {
    FungalSystem system;
    system.initialize();

    EXPECT_EQ(system.get_cycle_count(), 0);
}

TEST(SystemIntegrationTest, AnalyzeCodeAndRunCycle) {
    FungalSystem system;
    system.initialize();

    std::string code = "if (x > 5) { return x; }";
    system.analyze_code(code);
    system.run_reasoning_cycle(1);

    EXPECT_GT(system.get_cycle_count(), 0);
}

TEST(SystemIntegrationTest, ApplyMeasurementFeedback) {
    FungalSystem system;
    system.initialize();

    CodeStats stats;
    stats.max_nesting_depth = 3;
    stats.branch_count = 1;
    stats.function_count = 1;
    stats.token_count = 20;

    system.apply_measurement_feedback(stats);
}
