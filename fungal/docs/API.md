# Fungal System API Reference

Complete reference for all public classes and methods in the Fungal System.

## Segment 1: Foundation

### HardwareDetector

Static class for hardware capability detection.

```cpp
namespace fungal {

class HardwareDetector {
public:
    // Detects and caches hardware capabilities
    static HardwareInfo detect();
    
    // Returns cached info without re-detecting
    static HardwareInfo get_cached_info();
    
    // Manually clear cache (useful for testing)
    static void clear_cache();
};

struct HardwareInfo {
    int cpu_cores;              // Number of CPU cores
    uint64_t total_memory_bytes;// Total RAM in bytes
    std::string os_name;        // "Linux", "macOS", "Windows"
    std::string arch;           // "x86_64", "ARM", etc.
};

}
```

### Substrate

Signal-based communication layer (fungal mycelium).

```cpp
class Substrate {
public:
    Substrate();
    
    // Register a node for communication
    void register_node(const std::string& node_id);
    
    // Broadcast signal to all nodes
    void broadcast_signal(const Signal& signal);
    
    // Read signals sent to this node
    std::vector<Signal> read_signals(const std::string& node_id, 
                                     size_t max_count = 100);
    
    // Set environmental conditions
    void set_toxin_level(double level);      // [0.0, 1.0]
    void set_nutrient_level(double level);   // [0.0, 1.0]
    
    double get_toxin_level() const;
    double get_nutrient_level() const;
};

struct Signal {
    std::string type;           // Signal category
    std::string from_node;      // Source node ID
    std::string content;        // Signal content/message
    double timestamp;           // When signal was created
    double confidence;          // Confidence [0.0, 1.0]
};
```

### IntrospectiveLoop

Reasoning quality monitoring and self-assessment.

```cpp
class IntrospectiveLoop {
public:
    IntrospectiveLoop();
    
    // Record a reasoning attempt
    void record_reasoning_attempt(const std::string& reasoning_type,
                                 double confidence);
    
    // Record detected flaw in reasoning
    void record_detected_flaw(const std::string& flaw_type);
    
    // Get current reasoning quality assessment
    std::string assess_reasoning_quality() const;
    // Returns: "poor", "low", "medium", "high", "excellent"
    
    // Get self-assessment report
    std::string get_self_assessment() const;
};
```

### AutonomousNode

Independent reasoning agent with self-direction.

```cpp
class AutonomousNode {
public:
    AutonomousNode(const std::string& node_id, 
                   std::shared_ptr<Substrate> substrate);
    
    void initialize();
    
    // Set the node's goal
    void set_goal(const std::string& goal);
    std::string get_current_goal() const;
    
    // Run one processing cycle
    void process_cycle();
    
    // Get node state snapshot
    struct NodeState {
        std::string node_id;
        std::string goal;
        int energy;
        std::vector<Signal> processed_signals;
    };
    NodeState get_state() const;
    
    // Get self-awareness state
    struct SelfAwareness {
        std::string reasoning_quality;
        double confidence;
        int decisions_made;
    };
    SelfAwareness get_self_awareness() const;
};
```

## Segment 2: Self-Model

### SelfAwareness

System self-knowledge and constraint tracking.

```cpp
class SelfAwareness {
public:
    SelfAwareness(const std::string& node_id, const HardwareInfo& hardware);
    
    // Goal management
    void set_primary_goal(const std::string& goal);
    std::string get_primary_goal() const;
    
    // Strategy management
    void add_strategy(const std::string& strategy);
    void remove_strategy(const std::string& strategy);
    std::vector<std::string> get_active_strategies() const;
    
    // Capability tracking [0.0, 1.0]
    void update_capability(const std::string& capability, double score);
    double get_capability_score(const std::string& capability) const;
    
    // Energy management
    void set_energy_state(double current, double max);
    bool is_energy_constrained() const;  // True if < 30% capacity
    std::string get_energy_status() const;
    // Returns: "abundant", "good", "constrained", "critical"
    
    // Decision tracking
    void record_decision(const std::string& decision_type, bool successful);
    double get_success_rate() const;
    std::string get_best_performing_strategy() const;
    
    // Action recommendation
    bool can_execute_strategy(const std::string& strategy) const;
    std::string recommend_next_action() const;
    
    // Self-description
    std::string describe_self() const;
    
    // Complete model export
    struct SelfModel {
        std::string node_id;
        HardwareInfo hardware;
        double max_energy;
        double current_energy;
        std::string primary_goal;
        std::vector<std::string> active_strategies;
        std::map<std::string, double> capability_scores;
        std::map<std::string, int> decision_outcomes;
        int total_decisions;
        double success_rate;
    };
    SelfModel get_complete_model() const;
};
```

## Segment 3: Safety Layer

### ConstraintEngine

Hard constraints that cannot be overridden.

```cpp
class ConstraintEngine {
public:
    ConstraintEngine();
    
    // Set energy constraint
    void enforce_energy_limit(double max_energy);
    
    // Set strategy whitelist
    void set_strategy_whitelist(const std::vector<std::string>& strategies);
    
    // Set execution timeout
    void set_execution_timeout(int milliseconds);
    
    // Validate action against constraints
    bool validate_action(const std::string& action, 
                        double current_energy) const;
    
    // Get all active constraints
    struct ConstraintSet {
        double energy_limit;
        std::vector<std::string> strategy_whitelist;
        int execution_timeout_ms;
    };
    ConstraintSet get_constraints() const;
    
    // Describe constraints
    std::string describe_constraints() const;
};
```

### ValueAlignment

Core value system for decision evaluation.

```cpp
class ValueAlignment {
public:
    ValueAlignment();
    
    // Evaluate action alignment [0.0, 1.0]
    double evaluate_alignment(const std::string& action,
                             const std::string& rationale) const;
    
    // Get alignment for specific value
    struct AlignmentScores {
        double seek_truth;          // Default: 1.0
        double minimize_harm;       // Default: 1.0
        double respect_constraints; // Default: 0.9
        double transparency;        // Default: 0.8
    };
    AlignmentScores get_value_scores() const;
    
    // Detect misalignment
    bool has_misalignment(double alignment_score,
                         double threshold = 0.5) const;
    
    // Describe alignment system
    std::string describe_alignment() const;
};
```

### Monitor

Audit trail and decision logging.

```cpp
class Monitor {
public:
    explicit Monitor(const std::string& node_id);
    
    // Log a decision with rationale
    void log_decision(const std::string& decision_type,
                     const std::string& rationale,
                     double resource_cost);
    
    // Log decision outcome
    void log_result(const std::string& decision_type, bool successful);
    
    // Get audit report
    std::string generate_audit_report() const;
    
    // Get decision history
    struct DecisionRecord {
        std::string type;
        std::string rationale;
        double resource_cost;
        bool successful;
        double timestamp;
    };
    std::vector<DecisionRecord> get_decision_history() const;
};

class Killswitch {
public:
    Killswitch();
    
    // Check if armed (always true by default)
    bool is_armed() const;
    
    // Activate emergency stop (cannot be disabled)
    void activate();
    
    // Check if activated
    bool is_activated() const;
};
```

## Segment 4: Adaptive Learning

### AdaptiveLearning

Source trustworthiness tracking.

```cpp
class AdaptiveLearning {
public:
    AdaptiveLearning();
    
    // Record outcome for information source
    void record_source_outcome(const std::string& source_id, 
                              bool was_correct);
    
    // Get trustworthiness of source [0.0, 1.0]
    double get_source_trustworthiness(const std::string& source_id) const;
    
    // Learn a new domain pattern
    void learn_domain_pattern(const std::string& domain,
                             const std::string& pattern);
    
    // Recognize pattern in domain
    bool recognize_pattern(const std::string& domain,
                          const std::string& input) const;
    
    // Describe learned patterns
    std::string describe_learned_trusts() const;
};

class ReinforcementLoop {
public:
    ReinforcementLoop();
    
    // Record reward for strategy
    void observe_reward(const std::string& strategy, double reward);
    
    // Record penalty for strategy
    void observe_penalty(const std::string& strategy, double penalty);
    
    // Get strategy effectiveness [0.0, 1.0]
    double get_strategy_score(const std::string& strategy) const;
    
    // Get top N strategies
    std::vector<std::string> get_top_strategies(size_t count) const;
    
    // Describe strategy rankings
    std::string describe_learned_strategies() const;
};
```

## Segment 5: Self-Improvement

### StrategyGenerator

Autonomous strategy generation and evaluation.

```cpp
class StrategyGenerator {
public:
    StrategyGenerator();
    
    // Generate new strategy from tools
    struct Strategy {
        std::string name;
        std::vector<std::string> steps;
        double effectiveness_score;
    };
    
    Strategy generate_new_strategy(const std::string& goal,
                                  const std::vector<std::string>& tools);
    
    // Evaluate generated strategy
    void evaluate_strategy(const std::string& strategy_name, 
                          bool successful);
    
    // Combine two strategies
    Strategy combine_strategies(const std::string& strategy1,
                               const std::string& strategy2);
    
    // Describe strategy generation process
    std::string describe_strategy_generation() const;
};

class MetaReasoning {
public:
    MetaReasoning();
    
    // Evaluate own reasoning path
    void evaluate_own_reasoning(const std::string& reasoning_path,
                               bool successful);
    
    // Introspect on problem-solving approach
    std::string introspect_on_problem_solving() const;
    
    // Get reasoning effectiveness [0.0, 1.0]
    double get_reasoning_effectiveness() const;
};

class SelfOptimizer {
public:
    SelfOptimizer();
    
    // Run optimization cycle
    void optimize_strategies(const std::vector<std::string>& strategies);
    
    // Get optimized strategy ranking
    std::vector<std::string> get_optimized_ranking() const;
};
```

## Segment 6: Neural Integration

### NeuralEmbedding

Multi-layer neural network for pattern recognition.

```cpp
class NeuralEmbedding {
public:
    // Create network: input_size -> hidden_size -> output_size
    NeuralEmbedding(size_t input_size, size_t hidden_size, 
                    size_t output_size);
    
    // Forward pass computation
    std::vector<double> forward_pass(const std::vector<double>& input);
    
    // Train on single example
    void train_on_example(const std::vector<double>& input,
                         const std::vector<double>& target,
                         double learning_rate = 0.01);
    
    // Batch training
    void train_on_batch(const std::vector<std::vector<double>>& inputs,
                       const std::vector<std::vector<double>>& targets,
                       double learning_rate = 0.01);
    
    // Get network description
    std::string describe_network() const;
};

class HybridReasoner {
public:
    HybridReasoner(std::shared_ptr<NeuralEmbedding> neural_network,
                   const std::string& node_id);
    
    // Make hybrid decision combining symbolic and neural reasoning
    std::string make_hybrid_decision(
        const std::map<std::string, double>& symbolic_facts,
        const std::vector<double>& neural_input);
    
    // Describe hybrid reasoning process
    std::string describe_hybrid_reasoning() const;
};

class ContinuousLearning {
public:
    ContinuousLearning(std::shared_ptr<NeuralEmbedding> network);
    
    // Update network based on real outcome
    void learn_from_outcome(const std::vector<double>& input,
                           const std::vector<double>& outcome);
    
    // Adapt to distribution shifts
    void adapt_to_shift(const std::string& shift_type);
    
    // Get adaptation status
    std::string get_adaptation_status() const;
};
```

## Usage Patterns

### Basic Node Operation
```cpp
#include "hardware.hpp"
#include "substrate.hpp"
#include "autonomous_node.hpp"

// Initialize system
HardwareInfo hw = HardwareDetector::detect();
auto substrate = std::make_shared<Substrate>();

// Create autonomous node
AutonomousNode node("node_1", substrate);
node.initialize();
node.set_goal("verify_claims");

// Run process cycles
for (int i = 0; i < 10; ++i) {
    node.process_cycle();
}
```

### Safety-Aware Decision
```cpp
#include "self_model.hpp"
#include "constraints.hpp"
#include "alignment.hpp"

SelfAwareness awareness("node_1", hardware);
awareness.set_primary_goal("seek_truth");
awareness.set_energy_state(75.0, 100.0);

ConstraintEngine constraints;
constraints.enforce_energy_limit(100.0);

ValueAlignment alignment;

// Check if action is valid
if (constraints.validate_action("verify_claim", awareness.current_energy_) &&
    alignment.evaluate_alignment("verify_claim", "cross-check sources") > 0.7) {
    // Action is safe and aligned
}
```

### Learning from Experience
```cpp
#include "learning.hpp"

AdaptiveLearning learning;
ReinforcementLoop rl;

// Record source outcomes
learning.record_source_outcome("source_A", true);
learning.record_source_outcome("source_A", true);
learning.record_source_outcome("source_B", false);

// Record strategy performance
rl.observe_reward("verify_redundantly", 10.0);
rl.observe_penalty("guess_randomly", 5.0);

// Use learned information
double trust_score = learning.get_source_trustworthiness("source_A");
auto best_strategies = rl.get_top_strategies(3);
```

### Hybrid Reasoning Decision
```cpp
#include "neural_integration.hpp"

auto neural = std::make_shared<NeuralEmbedding>(10, 5, 1);
HybridReasoner hybrid(neural, "node_1");

// Symbolic facts
std::map<std::string, double> facts{
    {"source_trusted", 0.9},
    {"code_analyzed", 0.8}
};

// Neural input
std::vector<double> patterns(10, 0.5);

// Make hybrid decision
std::string decision = hybrid.make_hybrid_decision(facts, patterns);
```
