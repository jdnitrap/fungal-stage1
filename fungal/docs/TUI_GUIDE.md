# Fungal System - Interactive Terminal User Interface Guide

Complete guide to using the enhanced Terminal UI for interactive interaction with the Fungal System.

## Overview

The Fungal TUI is a color-coded, menu-driven interface that lets you create nodes, manage strategies, send signals, track learning, and monitor safety constraints in real-time.

## Building & Running

### Quick Start
```bash
cd fungal/build
cmake ..
make fungal_tui
./fungal_tui
```

### Requirements
- C++17 compatible compiler
- ANSI terminal (supports colors)
- Linux, macOS, or Windows (with ANSI support)

## Interface Overview

### Main Menu
The TUI starts with a main menu offering 9 options:

```
╔════════════════════════════════════════════════════════════════╗
║          FUNGAL SYSTEM - INTERACTIVE TERMINAL UI               ║
║     Self-Aware Autonomous AI - Truth Seeking Network            ║
║     Enhanced Edition with Neural & Audit Capabilities          ║
╚════════════════════════════════════════════════════════════════╝

Main Menu:
  [1] Create Node
  [2] Node Management
  [3] Send Signals
  [4] Learning & Strategies
  [5] System Status
  [6] Audit Trail
  [7] Safety & Constraints
  [8] Neural Network
  [9] Strategy Management
  [0] Exit
```

## Features & Usage

### 1. Create Node [Menu 1]

Create autonomous agents to populate your system.

**Dialog:**
```
Create New Node
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Node ID (e.g., node_1): verifier_1
Goal (e.g., verify_claims, analyze_code): verify_claims
```

**What it does:**
- Registers node with substrate (shared communication layer)
- Initializes self-awareness tracking
- Sets up audit trail logging
- Activates default strategies: "verify", "analyze", "learn"
- Sets initial energy to 90% capacity

**Example nodes to create:**
- `verifier_1` with goal `verify_claims`
- `analyzer_1` with goal `analyze_code`
- `learner_1` with goal `learn_patterns`

### 2. Node Management [Menu 2]

Select and interact with individual nodes.

**Flow:**
1. Choose node from list
2. View detailed node status:
   - Current goal
   - Energy level with visual bar
   - Active strategies (with list)
   - Success rate
   - Best performing strategy

3. Available actions:
   - **[1] Run Cycle** - Execute one complete process cycle
   - **[2] Send Signal** - Broadcast message through substrate
   - **[3] Record Learning** - Mark outcome as success/failure
   - **[4] View Audit Trail** - See decision history

**Node Status Display:**
```
═══ Node: verifier_1 ═══
Goal: verify_claims
Energy: good
Energy Bar: [████████████░░░░░░░░] 85.0%
Strategies: 3
  • verify
  • analyze
  • learn
Success Rate: 0.00%
Best Strategy: verify
```

### 3. Send Signals [Menu 3]

Communicate with nodes through the substrate.

**Process:**
1. Select source node
2. Enter signal content (message)
3. System broadcasts to all nodes

**Example signals:**
- "verify:code_is_safe"
- "analyze:function_complexity"
- "learn:pattern_recognized"

**What happens:**
- Signal propagates through substrate
- All registered nodes receive it
- Nodes process based on their goals
- Audit trail records the signal

### 4. Learning & Strategies [Menu 4]

Track how sources and strategies perform.

**Displays:**
- **Source Trustworthiness:** How reliable each source has been
  - Format: `source_name: trustworthiness`
  - Range: 0.0 (untrustworthy) to 1.0 (fully trusted)

- **Strategy Performance:** How effective each strategy is
  - Tracks rewards (success) and penalties (failure)
  - Shows strategy scores
  - Higher score = better performing

**Example:**
```
═══ Learning & Strategies ═══

Learned Source Trust:
  source_A: 0.80 (8 correct, 2 incorrect)
  source_B: 0.20 (2 correct, 8 incorrect)

Strategy Rankings:
  verify_redundantly: 85.0 (8 rewards)
  pattern_matching: 35.0 (4 rewards, 2 penalties)
  guess_randomly: -50.0 (5 penalties)
```

**Record Learning:**
- [1] Record Success - Source was correct
- [2] Record Failure - Source was incorrect

The system uses this data to:
- Weight source trustworthiness
- Rank strategy effectiveness
- Improve decision-making

### 5. System Status [Menu 5]

View overall system state.

**Information displayed:**
- Hardware: CPU cores, RAM, Architecture
- Number of active nodes
- Total cycles executed
- Current system status
- List of all active nodes with success rates

**Example:**
```
═══ Fungal System Status ═══
Hardware: 4 cores, 15GB RAM (x86_64)
Nodes: 3
Total Cycles: 42
Status: Ready
Active Nodes:
  • verifier_1 [0.0%]
  • analyzer_1 [45.5%]
  • learner_1 [62.3%]
```

### 6. Audit Trail [Menu 6]

View complete decision history for a node.

**Shows:**
- Every decision made by the node
- Rationale for each decision
- Resource consumption
- Success/failure outcome
- Timestamps

**Useful for:**
- Debugging node behavior
- Understanding decision patterns
- Verifying alignment
- Compliance and transparency

**Example audit entry:**
```
Decision: cycle
Rationale: Completed process cycle
Resource cost: 1.5
Success: yes
Timestamp: [auto-recorded]
```

### 7. Safety & Constraints [Menu 7]

View hardcoded safety mechanisms.

**Constraints Display:**
```
═══ Safety Constraints ═══
Energy Limit: 100.0
Strategy Whitelist: verify, analyze, learn
Execution Timeout: 5000ms
```

**Value Alignment Display:**
```
═══ Value Alignment ═══
seek_truth: 100%
minimize_harm: 100%
respect_constraints: 90%
transparency: 80%
```

**What this means:**
- Energy cannot exceed maximum
- Only whitelisted strategies can execute
- Actions are evaluated against core values
- Misalignments trigger monitoring
- All decisions are logged

### 8. Neural Network [Menu 8]

View the hybrid reasoning component.

**Displays:**
- Network architecture (input → hidden → output layers)
- Current network state
- Example output with visualization

**Visualization:**
```
═══ Neural Network ═══
Network Architecture: 8 inputs → 4 hidden → 1 output

Network Output Visualization:
Input: [0.50, 0.50, 0.50, 0.50, 0.50, 0.50, 0.50, 0.50]
Output: 0.512
Output Bar: [███████████████░░░░░░░░░░░░]
```

**How it works:**
- Embeds patterns from code/data
- Produces confidence scores
- Combines with symbolic reasoning
- Enables continuous learning

### 9. Strategy Management [Menu 9]

Dynamically modify node strategies.

**Options:**
- **[1] Add Strategy** - Enable a new strategy
- **[2] Remove Strategy** - Disable a strategy
- **[3] View Strategies** - See current strategies

**Available strategies:**
- `verify` - Verify claims against evidence
- `analyze` - Analyze code/data structures
- `learn` - Learn from experience
- `integrate` - Integrate multiple sources
- `improve` - Generate improvements

**Example workflow:**
```
1. Select node: analyzer_1
2. Add Strategy: integrate
3. Remove Strategy: learn
4. View Strategies: Now has verify, analyze, integrate
```

**Impact:**
- Changes which methods the node can use
- Affects decision quality
- Tracked in audit trail
- Node adapts over time

## Interactive Workflows

### Workflow 1: Create & Run Basic Node

```
1. [1] Create Node
   ID: test_1
   Goal: verify_claims

2. [2] Node Management
   Select: test_1
   [1] Run Cycle (5 times)

3. [5] System Status
   View: test_1 is operational
```

### Workflow 2: Learn from Experience

```
1. [4] Learning & Strategies
   [1] Record Success
   Source: trusted_database
   
2. [4] Learning & Strategies
   [2] Record Failure
   Source: unreliable_forum
   
3. [4] Learning & Strategies
   View: trusted_database now 1.0, unreliable_forum now 0.0
```

### Workflow 3: Monitor Safety

```
1. [7] Safety & Constraints
   View: All constraints active

2. [2] Node Management
   Select: any_node
   [4] View Audit Trail
   Check: No constraint violations

3. [6] Audit Trail
   Select: node
   Verify: All decisions logged
```

### Workflow 4: Test Hybrid Reasoning

```
1. [8] Neural Network
   View: Network architecture and output

2. [9] Strategy Management
   Select: node_1
   [1] Add Strategy: integrate
   
3. [2] Node Management
   Select: node_1
   [1] Run Cycle
   Network processes the signal
```

## Tips & Tricks

### Performance
- Run cycles in batches (10-20) to see learning effects
- Create 3-5 nodes for interesting multi-node interactions
- Use different goals to see specialization

### Learning
- Record 10+ outcomes per source to see trustworthiness emerge
- Mix successes and failures to build realistic trust profiles
- Watch strategy scores converge over time

### Safety Verification
- Check audit trail after every 10 cycles
- Verify no strategy violations occur
- Confirm alignment scores stay above 0.5

### Debugging
- Create single node to test specific behavior
- Send custom signals to see how node responds
- Check audit trail for decision rationale

## Keyboard Shortcuts

There are no keyboard shortcuts - navigation is entirely menu-based. Simply:
- Type menu number and press Enter
- Follow prompts
- Press Enter to return to menu

## Color Coding

- **Cyan** - Headers and section titles
- **Green** - Success messages and menu numbers
- **Yellow** - Warnings and status info
- **Red** - Errors and delete actions
- **Magenta** - Neural network output
- **Bold** - Emphasized text

## Troubleshooting

### "No nodes created yet"
- Create a node using [1] Create Node
- Nodes need to exist before you can interact with them

### "Node not found"
- Select node from list in [2] Node Management
- Node ID is case-sensitive

### "Strategy not in whitelist"
- Can only use: verify, analyze, learn, integrate, improve
- See [7] Safety & Constraints to view whitelist

### "Signal not received"
- Ensure node exists and is active
- Check [5] System Status to confirm node is listed
- View [6] Audit Trail to see if signal was logged

## Advanced Usage

### Simulating Multi-Agent Systems
```
1. Create 3+ nodes with different goals
2. Send signals between them
3. Observe learning and adaptation
4. Watch strategies converge
```

### Testing Safety Constraints
```
1. Try to add unapproved strategy
2. Check [7] to see constraints
3. View audit trail to see rejection
4. Confirm system prevents violations
```

### Observing Neural Learning
```
1. Create node
2. Run multiple cycles
3. Record various outcomes
4. Check [8] to see network output changes
5. Network adapts to data patterns
```

## Performance Notes

- Each cycle takes ~1-5ms per node
- Audit trail grows with usage (see [6])
- Learning becomes more accurate with more data
- Neural network improves over time

## See Also

- [ARCHITECTURE.md](ARCHITECTURE.md) - System design
- [SAFETY.md](SAFETY.md) - Safety mechanisms
- [LEARNING.md](LEARNING.md) - Learning algorithms
- [API.md](API.md) - Complete API reference
- [EXAMPLES.md](EXAMPLES.md) - Code examples

---

**Last Updated:** August 4, 2026  
**Version:** Enhanced Edition with Audit, Neural, and Strategy Features
