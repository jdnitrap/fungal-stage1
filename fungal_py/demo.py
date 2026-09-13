#!/usr/bin/env python3
"""Fungal v1 demo: control loop with ExternalOracle (real C++ compilation)."""

import sys
from control_loop import ControlLoop
from oracle import ExternalOracle
from strategy import PatternMatcherStrategy


def main():
    print("=== Fungal v1 Control Loop with EXTERNAL Oracle (Python) ===\n")

    # Create oracle (real C++ compilation)
    try:
        oracle = ExternalOracle(compiler="clang++", timeout_sec=5.0)
        print("Oracle: ExternalOracle (real C++ compilation with clang++)")
    except RuntimeError as e:
        print(f"Error: {e}")
        print("Make sure clang++ is installed and available.")
        return 1

    # Create strategy
    strategy = PatternMatcherStrategy()
    print("Strategy: PatternMatcher (weak heuristics for comparison)\n")

    # Create and initialize control loop
    loop = ControlLoop(oracle, strategy)
    loop.initialize_from_hardware()

    profile = loop.hardware_scheduler.get_profile()
    print("Hardware detected:")
    print(f"  CPU cores: {profile['cpu_cores']}")
    print(f"  Memory: {profile['memory_mb']} MB")
    print(f"  Architecture: {profile['arch']}")
    print(f"  Energy cost scale: {profile['energy_cost_scale']:.2f}x\n")

    # Test snippets
    test_snippets = [
        "int* p = nullptr; int x = *p;",
        "int* p = nullptr; if (p) { int x = *p; }",
        "int x; int y = x + 5;",
        "int x = 0; int y = x + 5;",
        "char buf[10]; strcpy(buf, very_long_string);",
        "char buf[10]; strncpy(buf, string, 9);",
        "int* p = new int(5); delete p; int z = *p;",
        "int* p = new int(5); int z = *p; delete p;",
        "int* p = new int(5);",
        "int x = INT_MAX; int y = x + 1;",
    ]

    print(f"Running {len(test_snippets)} test cycles with ExternalOracle...\n")
    print("=== PER-CYCLE LOGS ===\n")

    results = loop.run_cycles(test_snippets)

    correct = 0
    real_bugs = 0
    strategy_bugs = 0

    for i, result in enumerate(results, 1):
        oracle_says = "BUG" if result.oracle_ground_truth else "OK"
        strategy_says = "BUG" if result.strategy_claim else "OK"
        match = "CORRECT" if (result.prediction_correct and result.system_had_energy) else "WRONG"

        print(f"Cycle {i}:")
        print(f"  Code: {test_snippets[i-1][:70]}...")
        print(f"  Strategy claim: {strategy_says}")
        print(f"  Oracle label: {oracle_says}")
        print(f"  Outcome correct: {result.prediction_correct}")
        print(f"  Predicted μ (at cycle): {result.predicted_success * 100:.1f}%")
        print(f"  Empirical success rate: {loop.self_model.get_empirical_success_rate(0) * 100:.1f}%")
        print(f"  Calibration error: {loop.self_model.get_calibration_error(0):.3f}")
        print(f"  Energy spent this cycle: {result.energy_spent} units")
        print(f"  Had energy to run: {'yes' if result.system_had_energy else 'no'}")
        print(f"  Result: {match}\n")

        if result.oracle_ground_truth:
            real_bugs += 1
        if result.strategy_claim:
            strategy_bugs += 1
        if result.prediction_correct and result.system_had_energy:
            correct += 1

    print("=== SUMMARY ===\n")
    print("Results with ExternalOracle:")
    print(f"  Total cycles: {loop.total_cycles_run}")
    print(f"  Cycles ran: {loop.cycles_that_ran} (had energy)")
    if loop.cycles_that_ran > 0:
        accuracy = 100.0 * correct / loop.cycles_that_ran
    else:
        accuracy = 0.0
    print(f"  Correct predictions: {correct}/{loop.cycles_that_ran} = {accuracy:.1f}%")
    print(f"  Real bugs detected by oracle: {real_bugs}")
    print(f"  Bugs claimed by strategy: {strategy_bugs}")
    print(f"  Final energy budget: {loop.energy_budget.current_budget} units")
    print(f"  Final self-model μ: {loop.self_model.predict_success(0) * 100:.2f}%")
    print(f"  Empirical success rate: {loop.self_model.get_empirical_success_rate(0) * 100:.2f}%")
    print(f"  Final calibration error: {loop.self_model.get_calibration_error(0):.2f}")

    print("\nOracle Type: External (real C++ compilation)")
    print("Compiler: clang++ with -Wall -Wextra -fsanitize=undefined")
    print("Ground Truth: Actual compiler diagnostics (not pattern matching)")

    return 0


if __name__ == "__main__":
    sys.exit(main())
