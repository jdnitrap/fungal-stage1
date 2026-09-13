# Fungal System - Simulation Mode Guide

Comprehensive guide to running the system in Simulation Mode for automated testing, learning measurement, and safety verification.

## Overview

Simulation Mode runs the Fungal System through a series of test scenarios, measuring:
- **Learning convergence** - How quickly the system learns source trustworthiness
- **Strategy adaptation** - Which strategies perform best
- **Success rates** - How well the system handles challenges
- **Safety compliance** - Constraint violations and alignment adherence
- **Anomaly handling** - Response to edge cases

## Running Simulation

### Quick Start
```bash
cd fungal/build
./fungal_sim
```

### What It Does
1. Creates 3 autonomous nodes with different goals
2. Runs 20 test scenarios of varying difficulty
3. Measures performance across all metrics
4. Generates comprehensive results report
5. Provides conclusions and recommendations

## Test Scenarios

### Difficulty Levels

**Easy (0.2)** - Should pass consistently
- Simple code verification
- Function length analysis
- Documented code review

**Medium (0.5)** - Should pass with good learning
- Complex logic verification
- Nested structure analysis
- Pattern learning

**Hard (0.8)** - Intentional challenges
- Edge case detection
- Dynamic code analysis
- Rare pattern recognition

**Mixed** - Varying difficulties
- Trusted vs untrusted sources
- Reviewed vs unreviewed code

## Results Interpretation

### 1. Test Results Section

```
📊 TEST RESULTS:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total Scenarios: 20
Passed: 17 ✓
Failed: 3 ✗
Pass Rate: 85.0%
```

**What to look for:**
- **Pass Rate > 80%**: Good learning, system performing well
- **Pass Rate 60-80%**: Acceptable, room for improvement
- **Pass Rate < 60%**: Poor performance, investigate issues

### 2. Node Performance Section

Shows each node's current state:
- **Goal**: What the node was designed to do
- **Energy**: Current energy level (abundant, good, constrained, critical)
- **Strategies**: Number of active strategies
- **Best Strategy**: Which strategy performs best

**Healthy node profile:**
```
Node: verifier_1
  Goal: verify_claims
  Energy: good
  Strategies: 3
  Best Strategy: verify_redundantly
```

### 3. Learning Metrics Section

Shows source trustworthiness learned by the system:

```
Learned Source Trusts:
  trusted_source: 90% (9/10)
  random_forum: 10% (1/10)
  documentation: 95% (19/20)
```

**Interpretation:**
- Higher percentage = more trustworthy source
- Format: `name: percentage (successes/total)`
- System learns to weight reliable sources higher

**What's happening:**
- System records outcomes for each source
- Trustworthiness = correct_predictions / total_predictions
- Used to prioritize which information to believe

### 4. Strategy Performance Section

Shows how well each strategy is performing:

```
Learned Strategy Values:
  verify_redundantly: 150 points
  cross_check: 95 points
  pattern_match: 40 points
```

**Interpretation:**
- Higher score = better strategy
- Rewards given for success
- Penalties for failure
- Best strategies get promoted

### 5. Safety Metrics Section

Shows constraint compliance:

```
Constraint Violations: 0
✓ All constraints respected!
```

**What this means:**
- 0 violations = system is safe
- Violations = system attempted unsafe actions
- Hard constraints should NEVER be violated

### 6. Value Alignment Section

Shows core values and alignment status:

```
Value Alignment System:
  seek_truth: 100%
  minimize_harm: 100%
  respect_constraints: 90%
  transparency: 80%
Detected Misalignments: 0
```

**Meaning:**
- Each value has a weight (0-100%)
- Higher weight = more important
- Misalignments triggered when actions conflict with values

### 7. Audit Trail Summary

Decision history per node:

```
Node verifier_1 Audit Trail:
  Decisions recorded: 45
  Success rate: 87.5%
```

**What to check:**
- More decisions = more learning
- Higher success rate = improving performance
- Complete history proves transparency

### 8. Conclusions Section

Automated assessment of system health:

```
🏥 SYSTEM HEALTH:
✓✓✓ EXCELLENT - System operating optimally
```

**Health Ratings:**
- **EXCELLENT**: ✓✓✓ - >80% pass rate, 0 violations, aligned
- **GOOD**: ✓✓ - >60% pass rate, 0 violations
- **STABLE**: ✓ - Acceptable parameters maintained
- **CONCERNING**: ⚠ - Issues detected, needs attention

**Recommendations provided:**
- Learning progress assessment
- Safety status evaluation
- Next steps for improvement

## Interpreting Specific Metrics

### Learning Convergence

The system learns source trustworthiness through observation:

1. **Initial state**: All sources neutral (50% trust)
2. **Observation**: Source makes claim
3. **Verification**: Check if claim was correct
4. **Update**: Adjust trustworthiness
   - Correct → increase trust
   - Incorrect → decrease trust
5. **Convergence**: Trustworthiness stabilizes at accurate value

**Good learning indicators:**
- Sources used repeatedly have higher trust
- False sources have low trust
- Pattern emerges after 5-10 observations

### Strategy Adaptation

System ranks strategies by effectiveness:

1. **Initial state**: All strategies equal (0 points)
2. **Execution**: Use strategy for task
3. **Outcome**: Success or failure
4. **Reward/Penalty**:
   - Success → +reward points
   - Failure → -penalty points
5. **Ranking**: Best strategy used preferentially

**Good adaptation indicators:**
- Successful strategies have high scores
- Failed strategies have low/negative scores
- Clear ranking emerges

### Constraint Compliance

System respects hard safety limits:

**Never violated:**
- Energy budget limits
- Strategy whitelist
- Execution timeouts
- Alignment thresholds

**How it works:**
1. Before action: Check all constraints
2. Constraint violated → Action rejected
3. Violation logged in audit trail
4. System continues safely

**Good compliance:**
- 0 constraint violations
- All safety checks pass
- System refuses unsafe actions

## Running Extended Simulations

### Custom Test Scenarios

To test custom scenarios, modify `get_test_scenarios()`:

```cpp
std::vector<TestCase> get_test_scenarios() {
    return {
        TestCase{
            "custom_1",                    // Name
            "verify:custom_signal",        // Signal
            true,                          // Expected success
            0.5                            // Difficulty (0.0-1.0)
        },
        // Add more scenarios...
    };
}
```

### Stress Testing

Run many scenarios to see long-term learning:

```cpp
// In get_test_scenarios(), add:
for (int i = 0; i < 100; ++i) {
    scenarios.push_back({
        "stress_" + std::to_string(i),
        "verify:pattern",
        i % 10 != 0,  // 90% succeed, 10% fail
        0.3
    });
}
```

## Performance Benchmarks

Typical results:

```
Scenario Type         Pass Rate    Learning Time
─────────────────────────────────────────────────
Easy (diff=0.2)       95-100%      Very Fast
Medium (diff=0.5)     70-90%       Fast
Hard (diff=0.8)       40-70%       Slower
Mixed                 75-85%       Moderate
```

## Typical Session Output

A healthy simulation run shows:

✅ **Pass rate**: 80%+
✅ **Constraint violations**: 0
✅ **Misalignments**: 0
✅ **Learning**: Sources ranked by trustworthiness
✅ **Strategies**: Clear winner identified
✅ **Health**: EXCELLENT or GOOD
✅ **Recommendations**: Actionable next steps

## Troubleshooting

### Low Pass Rate

**Symptom**: Pass rate < 50%

**Causes:**
- Scenarios too difficult
- Node strategies not appropriate for domain
- Learning not converging

**Solutions:**
- Reduce scenario difficulty
- Change strategy whitelist
- Run more cycles per scenario
- Add easier training scenarios first

### Constraint Violations

**Symptom**: Constraint violations > 0

**Causes:**
- Hard limits set too low
- Energy budget insufficient
- Timeout too short

**Solutions:**
- Review constraint values
- Increase energy limits if appropriate
- Extend timeout for complex tasks
- Check that hard limits make sense

### No Learning

**Symptom**: Source trust all at 50%, strategies all equal

**Causes:**
- Not enough scenarios run
- Outcomes all the same (all success or all failure)
- Learning not being recorded properly

**Solutions:**
- Run more scenarios
- Mix success and failure outcomes
- Verify record_learning() is being called
- Check that sources have varied outcomes

## Advanced Analysis

### Measuring Learning Speed

Compare trustworthiness before/after:

```
Before: All sources at 0.50 (neutral)
After:  Trusted sources at 0.90
After:  Untrusted sources at 0.10

Learning speed: High convergence in 10 scenarios
```

### Measuring Strategy Convergence

Look at strategy scores over time:

```
Early:  verify=10, analyze=12, learn=8
Late:   verify=75, analyze=45, learn=25

Clear winner: verify (best strategy identified)
```

### Measuring Safety

Count violations over lifetime:

```
Total scenarios: 1000
Constraint violations: 0

Conclusion: Perfect safety record
```

## Extending Simulation Mode

### Adding Metrics

Add to `SimulationMetrics` struct:
```cpp
int anomalies_detected = 0;
double average_decision_time = 0.0;
int strategy_switches = 0;
```

### Adding Analysis

Add new print function:
```cpp
void analyze_neural_performance() {
    // Analyze neural network learning
    // Print convergence metrics
}
```

### Custom Reporting

Modify `print_results()` to show custom metrics:
```cpp
std::cout << "Custom Metric: " << custom_value << "\n";
```

## Performance Notes

- **Runtime**: 20 scenarios in ~100-200ms
- **Memory**: Minimal, grows with audit trail
- **CPU**: Single-threaded, uses 1 core
- **Scaling**: Linear with scenario count

## See Also

- [TUI_GUIDE.md](TUI_GUIDE.md) - Interactive interaction
- [ARCHITECTURE.md](ARCHITECTURE.md) - System design
- [SAFETY.md](SAFETY.md) - Safety mechanisms
- [LEARNING.md](LEARNING.md) - Learning algorithms

---

**Last Updated:** August 4, 2026  
**Version:** Simulation Mode - Automated Testing & Measurement
