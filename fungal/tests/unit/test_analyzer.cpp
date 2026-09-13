#include "../../src/layer1/analyzer.hpp"
#include <gtest/gtest.h>

using namespace fungal;

TEST(AnalyzerTest, AnalyzeSimpleCode) {
    std::string code = "if (x > 5) { return y; }";
    CodeStats stats = StructuralAnalyzer::analyze(code);

    EXPECT_GT(stats.token_count, 0);
    EXPECT_GE(stats.max_nesting_depth, 0);
}

TEST(AnalyzerTest, NestingDepth) {
    std::string code = "{ { { x = 5; } } }";
    CodeStats stats = StructuralAnalyzer::analyze(code);

    EXPECT_GT(stats.max_nesting_depth, 0);
}

TEST(AnalyzerTest, EmptyCode) {
    CodeStats stats = StructuralAnalyzer::analyze("");
    EXPECT_EQ(stats.token_count, 0);
}
