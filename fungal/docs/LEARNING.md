# Fungal System Learning Mechanisms

How the Fungal System learns from experience, improves strategies, and adapts to new environments.

## Learning Architecture

The system learns through three integrated mechanisms:

1. **Adaptive Learning** (Segment 4): Learn from information sources
2. **Reinforcement Learning** (Segment 4): Learn which strategies work
3. **Self-Improvement** (Segment 5): Generate and test new strategies
4. **Neural Learning** (Segment 6): Pattern recognition and adaptation

## Segment 4: Adaptive Learning

### Source Trustworthiness Tracking

The system learns to trust reliable information sources and distrust unreliable ones.

```cpp
class AdaptiveLearning {
public:
    // Record outcome for a source
    void record_source_outcome(const std::string& source_id, 
                              bool was_correct);
    
    // Get trustworthiness [0.0, 1.0]
    double get_source_trustworthiness(const std::string& source_id) const;
};
```

**How It Works:**

1. **Initial State**: All sources start with neutral trustworthiness (0.5)

2. **Observation**: System receives claim from source
   ```
   source_A claims: "Function complexity is high"
   ```

3. **Verification**: System verifies claim against evidence
   ```
   Evidence: Function has 50 lines, 5 nested loops, 10 branches
   → Claim verified: true
   ```

4. **Update**: Trustworthiness increases
   ```
   source_A: trustworthiness = 0.5 + 0.05 = 0.55
   ```

5. **Long-term Learning**:
   ```
   source_A correct: 8 times
   source_A incorrect: 2 times
   → trustworthiness = 8/10 = 0.80
   
   source_B correct: 2 times
   source_B incorrect: 8 times
   → trustworthiness = 2/10 = 0.20
   ```

**Metabolic Principle**: 
- True information is rewarded (more trustworthy sources)
- False information is penalized (less trustworthy sources)
- False claims metabolically starve (low trust = little influence)
- True claims are rewarded (high trust = influential)

### Domain Pattern Learning

```cpp
void learn_domain_pattern(const std::string& domain,
                         const std::string& pattern);

bool recognize_pattern(const std::string& domain,
                      const std::string& input) const;
```

**Example Pattern: Security Domain**

```
Domain: "security_vulnerabilities"

Patterns learned:
1. "strcpy without bounds check" → vulnerability
2. "SQL query with user input" → SQL injection risk
3. "Integer overflow in loop" → buffer overflow risk

When analyzing new code:
if (code matches pattern 2) {
    → Flag as potential SQL injection
    (confidence based on learned pattern strength)
}
```

**Domain Learning Process:**

1. **Collect examples**: See multiple instances of pattern
2. **Extract features**: What characteristics identify pattern?
3. **Generalize**: Learn rule that captures pattern
4. **Test**: Apply rule to new code
5. **Refine**: Update rule based on accuracy

## Reinforcement Learning

### Strategy Performance Tracking

```cpp
class ReinforcementLoop {
public:
    void observe_reward(const std::string& strategy, double reward);
    void observe_penalty(const std::string& strategy, double penalty);
    double get_strategy_score(const std::string& strategy) const;
    std::vector<std::string> get_top_strategies(size_t count) const;
};
```

**How Strategy Learning Works:**

1. **Initial State**: All strategies start equal
   ```
   strategy_A: score = 0.0
   strategy_B: score = 0.0
   strategy_C: score = 0.0
   ```

2. **Action**: Node tries strategy_A
   ```
   Goal: verify claim about code complexity
   Strategy: "count_lines_and_branches"
   Execution time: 0.5ms
   Success: verified complexity correctly
   ```

3. **Reward**: Strategy performed well
   ```
   Reward = 10.0 (success) * efficiency_bonus(2x)
   strategy_A: score = 0.0 + 20.0 = 20.0
   ```

4. **Different Outcome**: Strategy_C fails
   ```
   Goal: classify code security
   Strategy: "pattern_matching_only"
   Result: Missed vulnerability (false negative)
   Penalty = -15.0 (failure) * criticality_weight(2x)
   strategy_C: score = 0.0 - 30.0 = -30.0
   ```

5. **Ranking Over Time**
   ```
   After 100 decisions:
   strategy_A: 180.0 (best - 90% success rate)
   strategy_B: 50.0 (medium - 60% success rate)
   strategy_C: -70.0 (worst - 20% success rate)
   
   When choosing strategy:
   → Prefer strategy_A (highest score)
   → Use strategy_B as backup
   → Avoid strategy_C (negative score)
   ```

**Reward Mechanism:**
- Successful decision: +reward points
- Failed decision: -penalty points
- Higher score = better performing strategy
- Ranking determines which strategies are active

### Exploration vs Exploitation

```cpp
// High confidence in best strategy
if (get_strategy_score("best") > 50.0) {
    use_strategy("best");  // Exploit
} else {
    // Low confidence - explore alternatives
    try_alternate_strategy();  // Explore
}
```

## Segment 5: Self-Improvement

### Autonomous Strategy Generation

```cpp
class StrategyGenerator {
public:
    struct Strategy {
        std::string name;
        std::vector<std::string> steps;
        double effectiveness_score;
    };
    
    Strategy generate_new_strategy(const std::string& goal,
                                  const std::vector<std::string>& tools);
};
```

**How New Strategies Are Generated:**

1. **Identify Goal**: What problem needs solving?
   ```
   Goal: "verify_code_safety"
   Available tools: ["static_analysis", "test_execution", "review"]
   ```

2. **Combine Tools**:
   ```
   New strategy: "safety_verification"
   Steps:
   1. Run static_analysis
   2. If issues found, run test_execution
   3. If tests fail, request review
   4. Aggregate results
   ```

3. **Evaluate Strategy**:
   ```
   Test on known safe code: ✓ correctly identified as safe
   Test on known unsafe code: ✓ correctly identified as unsafe
   Effectiveness: 95%
   ```

4. **Rank Strategy**:
   ```
   If effectiveness > 80%:
       → Add to active strategies
   If effectiveness < 50%:
       → Discard
   Else:
       → Keep in reserve, may activate later
   ```

5. **Combine Successful Strategies**:
   ```
   strategy_A (verify_redundantly): 85% effective
   strategy_B (cross_check): 80% effective
   
   New strategy: "verify_and_cross_check"
   Steps: combine steps from A and B
   Effectiveness: 91% (synergy benefit)
   ```

### Meta-Reasoning: Learning How to Learn

```cpp
class MetaReasoning {
public:
    void evaluate_own_reasoning(const std::string& reasoning_path,
                               bool successful);
    std::string introspect_on_problem_solving() const;
};
```

**Self-Analysis Process:**

1. **Reflect on Decision**:
   ```
   Decision: "Claim X is true"
   Reasoning path: "verified with 3 sources"
   Outcome: Correct
   → Reasoning quality: high
   ```

2. **Identify Weaknesses**:
   ```
   Decision: "Code is optimized"
   Reasoning: "No obvious inefficiencies"
   Outcome: Incorrect (profiler found bottleneck)
   → Weakness: Not using performance analysis tools
   ```

3. **Adapt Reasoning**:
   ```
   Next time:
   - Include performance profiling step
   - Don't rely on manual inspection alone
   - Combine multiple analysis approaches
   ```

4. **Improve Future Decisions**:
   ```
   After 50 decisions, reasoning quality improves:
   Success rate: 60% → 75%
   → Learned effective reasoning approaches
   ```

## Segment 6: Neural Learning

### Neural Network Training

```cpp
class NeuralEmbedding {
public:
    void train_on_example(const std::vector<double>& input,
                         const std::vector<double>& target,
                         double learning_rate = 0.01);
};
```

**Training Process:**

1. **Forward Pass**: Compute network output
   ```
   Input: [code_complexity, function_calls, variables]
   Expected: [0.8]  (likely has bugs)
   Predicted: [0.5]
   Error: 0.3
   ```

2. **Backward Pass**: Adjust weights
   ```
   For each weight in network:
       weight -= learning_rate * gradient
   
   Learning rate: 0.01 (controlled update)
   ```

3. **Convergence**: Network learns pattern
   ```
   After 100 training examples:
   Error: 0.3 → 0.15 → 0.08 → 0.05
   Network accuracy improving
   ```

### Continuous Online Learning

```cpp
class ContinuousLearning {
public:
    void learn_from_outcome(const std::vector<double>& input,
                           const std::vector<double>& outcome);
    
    void adapt_to_shift(const std::string& shift_type);
};
```

**Adaptation to Environment Changes:**

1. **Detect Distribution Shift**:
   ```
   Normal distribution:
   → Most code has 10-50 functions
   → Complexity ranges 1-5
   
   New codebase:
   → Code has 100+ functions
   → Complexity ranges 8-12
   → Shift detected!
   ```

2. **Adapt Network**:
   ```
   Network predictions were calibrated for old distribution
   → Recalibrate weights
   → Retrain on new examples
   → Maintain old knowledge (transfer learning)
   ```

3. **Performance Recovery**:
   ```
   Old accuracy (old code): 85%
   New accuracy (new code at first): 40%
   After adaptation: 80% (recovered)
   ```

## Hybrid Learning (Combining All Systems)

### Complete Learning Example: Improving Code Analysis

**Day 1: Initial State**
```
System has no experience
- Source trustworthiness: neutral (0.5)
- Strategies: all equal performance (0.0)
- Neural network: random predictions
- Meta-reasoning: untrained
```

**Day 2: First Code Analysis**
```
Analyze Apache project:
1. Source "project_docs" claims: "uses producer-consumer pattern"
   Verify: ✓ Correct
   → Increase trustworthiness of "project_docs"

2. Try strategy "AST_analysis":
   Result: Correctly identified patterns
   → Reward strategy (score +10)

3. Neural network trained on 10 examples
   Early learning: accuracy 55%

4. Meta-reasoning: "AST approach worked well"
   → Remember to use AST analysis
```

**Day 3: Second Code Analysis**
```
Analyze Linux kernel:
1. Source "code_comments" claims: "function is deprecated"
   Verify: ✓ Correct
   → Increase trustworthiness

2. Try strategy "pattern_matching":
   Result: Missed some patterns
   → Penalize strategy (score -5)

3. Adaptive learning:
   "Apache vs Linux have different styles"
   → Learn kernel-specific patterns

4. Neural network retrained:
   Accuracy: 55% → 62%
```

**Day 7: After One Week**
```
Source trustworthiness learned:
- "project_docs": 0.85 (very trusted)
- "code_comments": 0.70 (somewhat trusted)
- "random_forum": 0.30 (low trust)

Strategy rankings:
1. "AST_analysis": 8.5/10 (best)
2. "semantic_analysis": 7.0/10 (good)
3. "pattern_matching": 4.5/10 (okay)
4. "naive_inspection": 2.0/10 (avoid)

Neural network accuracy: 75%

System recommendations:
- Use project documentation as primary source
- Apply AST analysis strategy
- Trust neural pattern matching moderately
- Always cross-check with semantic analysis
```

## Learning Limitations

### Learning Bottlenecks

1. **Limited Training Data**: Needs sufficient examples
   - Rare patterns take longer to learn
   - Novel situations require careful evaluation
   - Mitigation: Start conservative, learn gradually

2. **Feedback Delay**: May not know outcome immediately
   - Claim evaluation might take time
   - Mitigation: Track confidence, request early verification

3. **Biased Distributions**: Training data may not represent all scenarios
   - Learned patterns may not generalize
   - Mitigation: Test across diverse domains

### Overfitting Prevention

```cpp
// Don't over-learn from single source
double source_trust = learned_trust;
source_trust = 0.7 * source_trust + 0.3 * prior;  // Regularization

// Don't over-commit to strategy
double strategy_score = learned_score;
if (strategy_score > 0.9) {
    // Still try alternatives sometimes
    occasionally_explore_other_strategies();
}
```

## Testing Learning

### Unit Tests
```bash
# Test adaptive learning
ctest -R test_learning

# Test reinforcement
ctest -R test_reinforcement

# Test neural training
ctest -R test_neural
```

### Integration Test Example
```cpp
// Test complete learning cycle
AdaptiveLearning learning;
ReinforcementLoop rl;

// Train on examples
for (int i = 0; i < 100; ++i) {
    // Record outcomes
    learning.record_source_outcome("trusted_source", true);
    rl.observe_reward("good_strategy", 10.0);
}

// Verify learning occurred
assert(learning.get_source_trustworthiness("trusted_source") > 0.8);
assert(rl.get_strategy_score("good_strategy") > 50.0);
```

## Monitoring Learning Progress

```cpp
// Track learning metrics
class LearningMetrics {
    double source_trustworthiness_avg;  // Average trust in sources
    double strategy_effectiveness;      // Best strategy performance
    double neural_accuracy;             // Network prediction accuracy
    double improvement_rate;            // Learning speed
};
```

Nodes periodically report:
- How trustworthy are their information sources?
- How well are their strategies performing?
- How accurate are their neural predictions?
- Are they improving over time?

This information helps identify:
- Sources that need verification
- Strategies that need refinement
- Networks that need retraining
- Systems that are stagnating
