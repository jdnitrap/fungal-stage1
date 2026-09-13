# Fungal System Architecture

## Overview

The Fungal System is a self-aware, autonomous-oriented AI architecture inspired by fungal networks. It is hardware-aware, truth-seeking, and constrained by explicit safety/survival rules.

As of **2026-08-12**, the **live research spine** is:

> **Core control loop** (`fungal::core`) **+ optional Stage1 survival state**

Older six-segment foundation/safety/learning modules remain in-tree. Layer1/Layer2 provide analysis and multi-node claim dynamics. Production modules are an optional ops shell.

For Stage1 specifics see [STAGE1.md](STAGE1.md).

---

## Current spine: Core Loop + Stage1

### Components

| Component | Responsibility |
|---|---|
| `ControlLoop` | Orchestrates one evaluative cycle |
| `EnergyBudget` | Hard scarcity; spend / refund / load_state |
| `SelfModel` | Predict success; track accuracy vs calibration |
| `Strategy` | Produce a claim (e.g., bug / no-bug) |
| `Oracle` | Ground truth (`TestOracle`, `RealOracle`, `ExternalOracle`) |
| `HardwareAwareScheduler` | Detect hardware; scale cycle cost |
| `Stage1Store` | Checkpoint, audit JSONL, integrity hash, gates |

### Cycle

```text
Sense/Predict (SelfModel)
    → Stage1 precheck (if enabled): audit writable? recovery ok? budget ok?
    → Spend energy
    → Strategy claim
    → Oracle truth
    → Evaluate claim vs truth
    → Refund/penalty + SelfModel update
    → Stage1 checkpoint + audit (if enabled)
```

### Stage1 goal stack

1. Survive
2. Human-legible
3. Seek truth

Stage1 is **opt-in** via `initialize_stage1()`. Default demos keep non-persistent behavior.

### Persistence artifacts

```text
state/checkpoint.json
state/audit.jsonl
```

---

## Layer map

```text
┌──────────────────────────────────────────────────────────┐
│ Entry: fungal_stage1_demo / loop demos / tui / sim       │
└────────────────────────────┬─────────────────────────────┘
                             ▼
┌──────────────────────────────────────────────────────────┐
│ Core loop + optional Stage1                              │
│ EnergyBudget · SelfModel · Strategy · Oracle · Stage1Store│
└───────────────┬─────────────────────────┬────────────────┘
                ▼                         ▼
┌───────────────────────┐   ┌─────────────────────────────┐
│ Layer1                │   │ Layer2                      │
│ tokenize/analyze/     │──►│ nodes · energy · 12 phases  │
│ dictionary/verify/    │   │ voting                      │
│ build                 │   └─────────────────────────────┘
└───────────────────────┘
                │
                ▼
┌──────────────────────────────────────────────────────────┐
│ Foundation / Safety / Learning segments (parallel track) │
│ hardware · substrate · nodes · constraints · monitor ... │
└──────────────────────────────────────────────────────────┘
                             │
                             ▼
┌──────────────────────────────────────────────────────────┐
│ Production shell (optional): API, metrics, cluster, ...  │
└──────────────────────────────────────────────────────────┘
```

---

## Six-Segment Modular Design (legacy/parallel)

The system also contains six stacked segments from the earlier full-system design. These are still documented because the code exists; they are **not all wired into Stage1** yet.

### Segment 1: Foundation
Hardware detection, substrate signal passing, introspection, autonomous nodes.

### Segment 2: Self-Model
System self-knowledge (goals, strategies, capability scores, energy status). Note: the **core loop** uses `fungal::core::SelfModel` (predictive stats), which is related in spirit but a distinct implementation focused on task-type prediction/calibration.

### Segment 3: Safety Layer
ConstraintEngine, ValueAlignment, Monitor audit trail, Killswitch.

Stage1 adds a complementary, narrower safety posture: refuse action if audit/recovery/budget gates fail; no anti-operator persistence tricks.

### Segment 4: Adaptive Learning
Source trustworthiness and strategy reinforcement tracking.

### Segment 5: Self-Improvement
Strategy generation / meta-reasoning / optimization cycles.

### Segment 6: Neural Integration
Hybrid symbolic-neural path (present as module code).

---

## Layer1 / Layer2 / Bridge

### Layer1 — Knowledge builder
Tokenizer → Analyzer → Dictionary → Verifier → Builder.

### Layer2 — Claim/energy ecology
Nodes hold claims/energy/trust. Phases 1–12 evolve the network (decay, transfer, quarantine, removal, shocks, asymmetric trust, specialization, substrate learning, refinement, multi-network learning, uncertainty, hardware constraints). Voting aggregates outcomes.

### Bridge
Moves foundational claims from Layer1 into Layer2 and exports verified results back.

---

## Data flow (Stage1-enabled core cycle)

```text
code snippet
    → SelfModel.predict_success
    → precheck gates (Stage1)
    → EnergyBudget.spend_for_cycle
    → Strategy.apply → claim
    → Oracle.has_bug → truth
    → correct = (claim == truth)
    → EnergyBudget.refund_outcome(correct, cost_spent)
    → SelfModel.update_from_outcome
    → Stage1Store.save_checkpoint + append_audit
```

---

## Key Design Principles

1. **Autonomy with constraints** — independent cycling inside explicit gates
2. **Scarcity is real** — energy can stop work
3. **Legibility is required for Stage1 action** — no silent durable mode
4. **Truth is grounded** — oracle outcomes drive learning
5. **Distributed substrate ideas** — Layer2 models claim metabolism
6. **Hardware awareness** — cycle cost scales with resources
7. **Opt-in durability** — Stage1 does not break older demos by default
8. **Honest scope** — Stage1 ≠ full self-seeking autonomy

---

## File structure (core + stages)

```text
fungal/
├── include/core/
│   ├── control_loop.hpp
│   ├── stage1_state.hpp
│   ├── energy_budget.hpp
│   ├── self_model.hpp
│   ├── strategy.hpp
│   ├── hardware_aware_scheduler.hpp
│   └── *oracle*.hpp
├── src/core/
│   ├── control_loop.cpp
│   ├── stage1_state.cpp
│   ├── energy_budget.cpp
│   ├── self_model.cpp
│   ├── strategy.cpp
│   ├── control_loop_stage1_demo.cpp
│   └── other demos / oracles
├── src/layer1/
├── src/layer2/
├── src/bridge/
└── docs/
```

---

## Performance characteristics (approx)

- Hardware detection: fast, cached after first call
- Core cycle: lightweight relative to oracle cost
- Stage1 checkpoint/audit: disk-bound; designed for correctness over cleverness
- Memory: suitable for constrained hosts (Stage1 state is small vs model weights)

---

## Related docs

- [STAGE1.md](STAGE1.md)
- [SAFETY.md](SAFETY.md)
- [BUILDING.md](BUILDING.md)
- [../../talk.md](../../talk.md)
