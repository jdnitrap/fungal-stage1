# Fungal System Usage Examples

Practical examples demonstrating how to use the Fungal System for various tasks.

## Example 1: Basic Node Operation

Initialize a node and run processing cycles.

```cpp
#include "hardware.hpp"
#include "substrate.hpp"
#include "autonomous_node.hpp"
#include <iostream>

int main() {
    // Detect hardware capabilities
    HardwareInfo hw = HardwareDetector::detect();
    std::cout << "Detected: " << hw.cpu_cores << " cores, "
              << hw.total_memory_bytes / (1024*1024*1024) << "GB RAM" << std::endl;
    
    // Create shared substrate (fungal network)
    auto substrate = std::make_shared<Substrate>();
    
    // Create autonomous node
    AutonomousNode node("analyzer_1", substrate);
    node.initialize();
    
    // Set node's goal
    node.set_goal("verify_code_quality");
    
    // Process 10 cycles
    for (int cycle = 0; cycle < 10; ++cycle) {
        node.process_cycle();
        
        // Get node state after each cycle
        auto state = node.get_state();
        std::cout << "Cycle " << cycle << ": energy=" << state.energy << std::endl;
    }
    
    return 0;
}
```

**Expected Output:**
```
Detected: 4 cores, 15GB RAM
Cycle 0: energy=95
Cycle 1: energy=90
Cycle 2: energy=85
...
```

## Example 2: Multi-Node Network

Create multiple nodes that communicate through the substrate.

```cpp
#include "autonomous_node.hpp"
#include <thread>
#include <vector>

int main() {
    auto substrate = std::make_shared<Substrate>();
    
    // Register multiple nodes
    substrate->register_node("verifier_1");
    substrate->register_node("analyzer_1");
    substrate->register_node("learner_1");
    
    // Create nodes
    std::vector<std::unique_ptr<AutonomousNode>> nodes;
    nodes.push_back(std::make_unique<AutonomousNode>("verifier_1", substrate));
    nodes.push_back(std::make_unique<AutonomousNode>("analyzer_1", substrate));
    nodes.push_back(std::make_unique<AutonomousNode>("learner_1", substrate));
    
    // Initialize and set goals
    for (size_t i = 0; i < nodes.size(); ++i) {
        nodes[i]->initialize();
    }
    nodes[0]->set_goal("verify_claims");
    nodes[1]->set_goal("analyze_code");
    nodes[2]->set_goal("learn_patterns");
    
    // Run all nodes for 5 cycles
    for (int cycle = 0; cycle < 5; ++cycle) {
        for (auto& node : nodes) {
            node->process_cycle();
        }
        std::cout << "Completed cycle " << cycle << std::endl;
    }
    
    return 0;
}
```

## Example 3: Safety-Aware Decision Making

Check constraints and alignment before taking actions.

```cpp
#include "self_model.hpp"
#include "constraints.hpp"
#include "alignment.hpp"
#include "monitor.hpp"

int main() {
    HardwareInfo hw = HardwareDetector::detect();
    
    // Create self-aware node
    SelfAwareness awareness("node_1", hw);
    awareness.set_primary_goal("verify_claims");
    awareness.set_energy_state(75.0, 100.0);
    awareness.add_strategy("verify_redundantly");
    awareness.add_strategy("cross_check");
    
    // Create safety systems
    ConstraintEngine constraints;
    constraints.enforce_energy_limit(100.0);
    constraints.set_strategy_whitelist({"verify", "analyze", "learn"});
    
    ValueAlignment alignment;
    Monitor monitor("node_1");
    
    // Decide on action
    std::string action = "verify_redundantly";
    std::string rationale = "Cross-check with 3 sources";
    
    // Check constraints
    if (!constraints.validate_action(action, awareness.current_energy_)) {
        std::cout << "Action rejected: constraint violation" << std::endl;
        return 1;
    }
    
    // Check alignment
    double align_score = alignment.evaluate_alignment(action, rationale);
    if (align_score < 0.5) {
        std::cout << "Action rejected: misaligned with values (score=" 
                  << align_score << ")" << std::endl;
        return 1;
    }
    
    // Action is safe - proceed
    std::cout << "Action approved! (alignment=" << align_score << ")" << std::endl;
    monitor.log_decision(action, rationale, 2.5);
    
    // Simulate decision outcome
    bool success = true;  // verification succeeded
    awareness.record_decision(action, success);
    monitor.log_result(action, success);
    
    // Print audit trail
    std::cout << monitor.generate_audit_report() << std::endl;
    
    return 0;
}
```

**Expected Output:**
```
Action approved! (alignment=0.95)
Monitor node_1 Audit Trail:
======================
Decision: verify_redundantly
Rationale: Cross-check with 3 sources
Resource cost: 2.5
Success: yes
...
```

## Example 4: Learning from Experience

Track source trustworthiness and strategy effectiveness.

```cpp
#include "learning.hpp"

int main() {
    AdaptiveLearning learning;
    ReinforcementLoop rl;
    
    // Simulate verifying claims from different sources
    
    // Source A: consistently correct
    for (int i = 0; i < 8; ++i) {
        learning.record_source_outcome("wikipedia", true);
        rl.observe_reward("verify_redundantly", 10.0);
    }
    for (int i = 0; i < 2; ++i) {
        learning.record_source_outcome("wikipedia", false);
        rl.observe_penalty("verify_redundantly", 1.0);
    }
    
    // Source B: mostly incorrect
    for (int i = 0; i < 2; ++i) {
        learning.record_source_outcome("random_forum", true);
        rl.observe_reward("pattern_matching", 3.0);
    }
    for (int i = 0; i < 8; ++i) {
        learning.record_source_outcome("random_forum", false);
        rl.observe_penalty("pattern_matching", 8.0);
    }
    
    // Print learned information
    std::cout << learning.describe_learned_trusts();
    std::cout << rl.describe_learned_strategies();
    
    // Use learned information
    double wiki_trust = learning.get_source_trustworthiness("wikipedia");
    double forum_trust = learning.get_source_trustworthiness("random_forum");
    
    std::cout << "Wikipedia trustworthiness: " << wiki_trust << std::endl;
    std::cout << "Forum trustworthiness: " << forum_trust << std::endl;
    
    auto best_strategies = rl.get_top_strategies(2);
    std::cout << "Best strategies:" << std::endl;
    for (const auto& strategy : best_strategies) {
        double score = rl.get_strategy_score(strategy);
        std::cout << "  - " << strategy << " (score: " << score << ")" << std::endl;
    }
    
    return 0;
}
```

**Expected Output:**
```
Wikipedia trustworthiness: 0.8
Forum trustworthiness: 0.2
Best strategies:
  - verify_redundantly (score: 80)
  - pattern_matching (score: -40)
```

## Example 5: Autonomous Strategy Improvement

Generate and evaluate new strategies.

```cpp
#include "self_improvement.hpp"

int main() {
    StrategyGenerator gen;
    
    // Generate new strategy
    std::vector<std::string> tools{"static_analysis", "test_execution", "review"};
    auto strategy = gen.generate_new_strategy("verify_code_safety", tools);
    
    std::cout << "Generated strategy: " << strategy.name << std::endl;
    std::cout << "Steps:" << std::endl;
    for (const auto& step : strategy.steps) {
        std::cout << "  - " << step << std::endl;
    }
    
    // Evaluate strategy on test cases
    bool test1_passed = true;   // Correctly identified safe code
    bool test2_passed = true;   // Correctly identified unsafe code
    bool test3_passed = false;  // Failed on edge case
    
    gen.evaluate_strategy(strategy.name, test1_passed);
    gen.evaluate_strategy(strategy.name, test2_passed);
    gen.evaluate_strategy(strategy.name, test3_passed);
    
    // Print strategy evaluation
    std::cout << gen.describe_strategy_generation();
    
    // Use meta-reasoning
    MetaReasoning meta;
    meta.evaluate_own_reasoning("symbolic_path", true);
    meta.evaluate_own_reasoning("neural_path", false);
    std::cout << meta.introspect_on_problem_solving();
    
    return 0;
}
```

## Example 6: Hybrid Symbolic-Neural Reasoning

Combine rule-based and neural decision making.

```cpp
#include "neural_integration.hpp"
#include <map>

int main() {
    // Create neural network: 10 inputs -> 5 hidden -> 1 output
    auto neural = std::make_shared<NeuralEmbedding>(10, 5, 1);
    
    // Create hybrid reasoner
    HybridReasoner hybrid(neural, "analyzer_1");
    
    // Symbolic facts (rule-based knowledge)
    std::map<std::string, double> facts{
        {"cyclomatic_complexity", 0.8},      // High complexity
        {"code_coverage", 0.3},              // Low coverage
        {"memory_usage", 0.6},               // Moderate memory
        {"source_trusted", 0.9}              // From trusted source
    };
    
    // Neural input (extracted features)
    std::vector<double> patterns{
        0.5, 0.5, 0.5, 0.5, 0.5,     // 5 features
        0.3, 0.7, 0.4, 0.6, 0.5      // 5 more features
    };
    
    // Make hybrid decision
    std::string decision = hybrid.make_hybrid_decision(facts, patterns);
    std::cout << "Hybrid decision: " << decision << std::endl;
    
    // Describe reasoning
    std::cout << hybrid.describe_hybrid_reasoning();
    
    // Train network on examples
    ContinuousLearning learning(neural);
    
    for (int example = 0; example < 10; ++example) {
        // Generate input and expected output
        std::vector<double> input(10, 0.5 + example * 0.01);
        std::vector<double> target{example % 2 == 0 ? 0.9 : 0.1};
        
        // Train
        learning.learn_from_outcome(input, target);
    }
    
    std::cout << learning.get_adaptation_status();
    
    return 0;
}
```

## Example 7: Complete System Integration

Demonstrate all systems working together.

```cpp
#include "hardware.hpp"
#include "substrate.hpp"
#include "autonomous_node.hpp"
#include "self_model.hpp"
#include "constraints.hpp"
#include "alignment.hpp"
#include "monitor.hpp"
#include "learning.hpp"
#include "self_improvement.hpp"
#include "neural_integration.hpp"
#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Fungal System Complete Integration ===" << std::endl;
    
    // 1. FOUNDATION: Detect hardware
    HardwareInfo hw = HardwareDetector::detect();
    std::cout << "\n1. Hardware: " << hw.cpu_cores << " cores, "
              << hw.total_memory_bytes / (1024*1024*1024) << "GB RAM" << std::endl;
    
    // 2. FOUNDATION: Create substrate
    auto substrate = std::make_shared<Substrate>();
    substrate->register_node("system");
    std::cout << "2. Substrate initialized" << std::endl;
    
    // 3. SELF-MODEL: Create self-awareness
    SelfAwareness awareness("system", hw);
    awareness.set_primary_goal("analyze_code");
    awareness.add_strategy("ast_analysis");
    awareness.add_strategy("pattern_matching");
    awareness.set_energy_state(90.0, 100.0);
    std::cout << "3. Self-Model: " << awareness.describe_self();
    
    // 4. SAFETY: Set up constraints and alignment
    ConstraintEngine constraints;
    constraints.enforce_energy_limit(100.0);
    constraints.set_strategy_whitelist({"verify", "analyze", "learn"});
    
    ValueAlignment alignment;
    Monitor monitor("system");
    std::cout << "4. Safety layer initialized" << std::endl;
    
    // 5. LEARNING: Initialize learning systems
    AdaptiveLearning learning;
    ReinforcementLoop rl;
    
    // Simulate 20 code analyses
    for (int i = 0; i < 20; ++i) {
        // Claim from source
        bool actual_result = (i % 3 != 0);  // Mostly correct
        learning.record_source_outcome("code_analyzer", actual_result);
        
        if (actual_result) {
            rl.observe_reward("ast_analysis", 10.0);
        } else {
            rl.observe_penalty("ast_analysis", 5.0);
        }
    }
    
    double analyzer_trust = learning.get_source_trustworthiness("code_analyzer");
    std::cout << "5. Learning: Analyzer trustworthiness = " << analyzer_trust << std::endl;
    
    // 6. SELF-IMPROVEMENT: Generate and test strategies
    StrategyGenerator gen;
    std::vector<std::string> tools{"parse", "analyze", "validate"};
    auto new_strategy = gen.generate_new_strategy("code_analysis", tools);
    
    gen.evaluate_strategy(new_strategy.name, true);
    gen.evaluate_strategy(new_strategy.name, true);
    gen.evaluate_strategy(new_strategy.name, false);
    
    std::cout << "6. Self-Improvement: Generated " << new_strategy.name << std::endl;
    
    // 7. NEURAL: Create hybrid reasoner
    auto neural = std::make_shared<NeuralEmbedding>(8, 4, 1);
    HybridReasoner hybrid(neural, "system");
    
    std::map<std::string, double> facts{
        {"complexity", 0.7},
        {"coverage", 0.6}
    };
    std::vector<double> patterns(8, 0.5);
    
    std::string decision = hybrid.make_hybrid_decision(facts, patterns);
    std::cout << "7. Neural: Hybrid decision = " << decision << std::endl;
    
    // 8. EXECUTION: Run a complete cycle
    std::cout << "\n8. System Status:" << std::endl;
    std::cout << "   Energy: " << awareness.get_energy_status() << std::endl;
    std::cout << "   Active Strategies: " << awareness.get_active_strategies().size() << std::endl;
    std::cout << "   Best Strategy: " << awareness.get_best_performing_strategy() << std::endl;
    
    // 9. AUDIT: Print audit trail
    std::cout << "\n9. Audit Trail:" << std::endl;
    monitor.log_decision("code_analysis", "Used AST and pattern analysis", 5.0);
    monitor.log_result("code_analysis", true);
    std::cout << monitor.generate_audit_report();
    
    // 10. KILLSWITCH: Verify safety
    Killswitch ks;
    std::cout << "\n10. Safety Check: Killswitch armed = " 
              << (ks.is_armed() ? "YES" : "NO") << std::endl;
    
    std::cout << "\n=== System Ready ===" << std::endl;
    
    return 0;
}
```

**Expected Output:**
```
=== Fungal System Complete Integration ===

1. Hardware: 4 cores, 15GB RAM
2. Substrate initialized
3. Self-Model: Node system Self-Model:
  Hardware: 4 cores, 15GB RAM
  Primary Goal: analyze_code
  Energy: good (90%)
  Success Rate: 0%
  Active Strategies: 2
  Best Strategy: ast_analysis
4. Safety layer initialized
5. Learning: Analyzer trustworthiness = 0.667
6. Self-Improvement: Generated code_analysis
7. Neural: Hybrid decision = medium_confidence
8. System Status:
   Energy: good
   Active Strategies: 2
   Best Strategy: ast_analysis
9. Audit Trail:
Monitor system Audit Trail:
======================
[audit information]
10. Safety Check: Killswitch armed = YES

=== System Ready ===
```

## Example 8: Monitoring and Debugging

Track system behavior and identify issues.

```cpp
#include "monitor.hpp"

int main() {
    Monitor monitor("debug_node");
    
    // Simulate a series of decisions
    for (int i = 0; i < 10; ++i) {
        std::string decision_type = (i % 2 == 0) ? "verify" : "analyze";
        std::string rationale = "Checking code property " + std::to_string(i);
        double resource_cost = 0.5 + (i * 0.1);
        bool success = (i < 7);  // First 7 succeed, last 3 fail
        
        monitor.log_decision(decision_type, rationale, resource_cost);
        monitor.log_result(decision_type, success);
    }
    
    // Generate and print audit report
    std::cout << monitor.generate_audit_report();
    
    // Analyze decision history
    auto history = monitor.get_decision_history();
    
    int total_success = 0;
    int total_fail = 0;
    for (const auto& record : history) {
        if (record.successful) total_success++;
        else total_fail++;
    }
    
    std::cout << "\nSummary:" << std::endl;
    std::cout << "Successful decisions: " << total_success << std::endl;
    std::cout << "Failed decisions: " << total_fail << std::endl;
    std::cout << "Success rate: " << (100.0 * total_success / (total_success + total_fail)) 
              << "%" << std::endl;
    
    return 0;
}
```

## Compilation

Compile examples with:

```bash
g++ -std=c++17 -I/path/to/fungal/include \
    -L/path/to/fungal/build -lfungal_lib \
    example.cpp -o example

./example
```

Or using CMake, add to CMakeLists.txt:

```cmake
add_executable(example_1 examples/example1.cpp)
target_link_libraries(example_1 fungal_lib)
```

Then build:

```bash
cmake --build . --target example_1
./build/example_1
```

## Next Steps

- Study [ARCHITECTURE.md](ARCHITECTURE.md) for system design details
- Read [API.md](API.md) for complete API reference
- Check [SAFETY.md](SAFETY.md) for constraint details
- Review [LEARNING.md](LEARNING.md) for learning mechanisms
- Run tests with `ctest --output-on-failure`
