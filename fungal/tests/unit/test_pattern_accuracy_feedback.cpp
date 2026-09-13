// Verifies the fix for a previously-documented gap: SelfModel's
// accuracy tracking used to be a dead end — Strategy::apply() never
// read it back, so nothing about the strategy's behavior actually
// adapted from experience. This test proves the closed loop added in
// strategy.hpp/.cpp + control_loop.cpp genuinely works, at two levels:
//
//   1. Strategy level (in isolation): a pattern type with a proven-good
//      track record in SelfModel yields higher confidence than the same
//      pattern type with a proven-bad track record, for the identical
//      trigger snippet.
//   2. Full ControlLoop level (end to end): running real cycles through
//      the actual control loop against a real Oracle measurably changes
//      SelfModel's per-pattern-type accuracy, and a subsequent identical
//      snippet's strategy_confidence reflects that change.
//
// Plain main()-based test (no GTest dependency), matching the existing
// style of tests/system/test_self_aware_system.cpp in this codebase.

#include "core/strategy.hpp"
#include "core/self_model.hpp"
#include "core/control_loop.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace fungal::core;

namespace {

int g_failures = 0;

void check(bool condition, const std::string& label) {
    if (condition) {
        std::cout << "[PASS] " << label << "\n";
    } else {
        std::cout << "[FAIL] " << label << "\n";
        g_failures++;
    }
}

// A snippet that triggers exactly the null-deref pattern and nothing else.
const std::string kNullDerefSnippet = "int* p = nullptr; int x = *p;";

// A tiny real Oracle for the end-to-end test: this specific snippet really
// does have a bug (dereferencing a null pointer with no guard).
class AlwaysBugOracle : public Oracle {
public:
    bool has_bug(const std::string& /*code_snippet*/) override { return true; }
};

}  // namespace

int main() {
    std::cout << "=== Pattern-type accuracy feedback loop ===\n\n";

    // --- Part 1: Strategy level, in isolation ---
    {
        std::cout << "-- Part 1: Strategy::apply() reads SelfModel's per-pattern accuracy --\n";

        PatternMatcherStrategy strategy;

        SelfModel fresh_model;
        StrategyResult baseline = strategy.apply(kNullDerefSnippet, fresh_model);
        check(baseline.claim, "baseline: null-deref snippet still triggers a claim");
        check(baseline.triggered_patterns.size() == 1 &&
              baseline.triggered_patterns[0] == kNullDerefTaskId,
              "baseline: exactly the null-deref pattern fired");

        // Simulate a strong track record for this pattern: 20 confirmed-correct outcomes.
        SelfModel good_track_record;
        for (int i = 0; i < 20; ++i) {
            good_track_record.update_from_outcome(kNullDerefTaskId, /*outcome_correct=*/true, 0.8);
        }
        double good_accuracy = good_track_record.get_accuracy(kNullDerefTaskId);
        check(good_accuracy > 0.9, "good track record: SelfModel accuracy climbed above 0.9 (got " +
              std::to_string(good_accuracy) + ")");

        // Simulate a poor track record for the same pattern: 20 confirmed-wrong outcomes.
        SelfModel bad_track_record;
        for (int i = 0; i < 20; ++i) {
            bad_track_record.update_from_outcome(kNullDerefTaskId, /*outcome_correct=*/false, 0.8);
        }
        double bad_accuracy = bad_track_record.get_accuracy(kNullDerefTaskId);
        check(bad_accuracy < 0.1, "bad track record: SelfModel accuracy dropped below 0.1 (got " +
              std::to_string(bad_accuracy) + ")");

        StrategyResult with_good_record = strategy.apply(kNullDerefSnippet, good_track_record);
        StrategyResult with_bad_record = strategy.apply(kNullDerefSnippet, bad_track_record);

        std::cout << "  baseline (0.5 default) confidence:  " << baseline.strategy_confidence << "\n";
        std::cout << "  good track record confidence:       " << with_good_record.strategy_confidence << "\n";
        std::cout << "  bad track record confidence:        " << with_bad_record.strategy_confidence << "\n";

        check(with_good_record.strategy_confidence > baseline.strategy_confidence,
              "a pattern with a proven-good record yields HIGHER confidence than the neutral default");
        check(with_bad_record.strategy_confidence < baseline.strategy_confidence,
              "a pattern with a proven-bad record yields LOWER confidence than the neutral default");
        check(with_good_record.strategy_confidence > with_bad_record.strategy_confidence,
              "good-record confidence strictly exceeds bad-record confidence for the identical snippet");
    }

    std::cout << "\n";

    // --- Part 2: full ControlLoop, end to end ---
    {
        std::cout << "-- Part 2: real ControlLoop cycles measurably update SelfModel, "
                     "which measurably changes Strategy's next confidence --\n";

        auto oracle = std::make_shared<AlwaysBugOracle>();
        auto strategy = std::make_shared<PatternMatcherStrategy>();
        ControlLoop loop(oracle, strategy);

        double accuracy_before = loop.self_model().get_accuracy(kNullDerefTaskId);
        check(std::abs(accuracy_before - 0.5) < 1e-9,
              "before any cycles: null-deref accuracy is the untested default (0.5)");

        CycleResult first = loop.run_cycle(kNullDerefSnippet);
        check(first.system_had_energy, "first cycle had enough energy to run");
        check(first.prediction_correct,
              "first cycle: claim (bug) matched the real Oracle's truth (bug)");
        double confidence_after_one_cycle = first.strategy_confidence;

        // Run several more correct cycles so the pattern's track record climbs further.
        for (int i = 0; i < 15; ++i) {
            loop.run_cycle(kNullDerefSnippet);
        }

        double accuracy_after = loop.self_model().get_accuracy(kNullDerefTaskId);
        std::cout << "  null-deref accuracy: " << accuracy_before << " -> " << accuracy_after << "\n";
        check(accuracy_after > accuracy_before + 0.3,
              "after 16 real cycles through ControlLoop, SelfModel's null-deref accuracy rose substantially");

        CycleResult later = loop.run_cycle(kNullDerefSnippet);
        std::cout << "  strategy_confidence: cycle 1 = " << confidence_after_one_cycle
                  << ", cycle 17 = " << later.strategy_confidence << "\n";
        check(later.strategy_confidence > confidence_after_one_cycle,
              "strategy_confidence for the identical snippet is measurably higher later in the run, "
              "purely as a result of ControlLoop's own accumulated experience");
    }

    std::cout << "\n";
    if (g_failures == 0) {
        std::cout << "ALL PATTERN-ACCURACY FEEDBACK CHECKS PASSED\n";
        return 0;
    }
    std::cout << g_failures << " CHECK(S) FAILED\n";
    return 1;
}
