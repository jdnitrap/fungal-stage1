# Stage1 Survival Architecture

Stage1 is the first durable autonomy step for Fungal.
It is **not** full independence. It is a constrained process that can continue across runs with inspectable state.

## Goal stack

1. **Survive** — process continuity, budget solvency, recovery path
2. **Human-legible** — mandatory audit + checkpoint integrity
3. **Seek truth** — oracle-grounded feedback; success refund / failure penalty

Conflict rule: higher goals override lower ones.

## Explicit non-goals (Stage1)

- No anti-kill / process immortalization against the operator
- No self-copy to other machines
- No hiding from inspection
- No acquiring money, accounts, or new hardware

Core rules stored in identity:
`no_anti_kill`, `no_self_copy`, `no_hide_from_operator`, `audit_every_cycle`

## What “alive” means

Stage1 treats the system as alive when:

- `survival.alive == true`
- audit log is writable
- recovery path (`state/`) is available
- budget is non-negative and above configured minimum for new work

If audit or recovery fails, Stage1 enters **safe mode** and refuses further actions.

## Components

| Piece | Path | Role |
|---|---|---|
| Stage1Store | `include/core/stage1_state.hpp`, `src/core/stage1_state.cpp` | checkpoint, audit, hash, gates |
| ControlLoop hooks | `include/core/control_loop.hpp`, `src/core/control_loop.cpp` | opt-in Stage1 cycle integration |
| EnergyBudget | `include/core/energy_budget.hpp` | scarcity + `load_state` restore |
| SelfModel | `include/core/self_model.hpp` | predict/update + export/import |
| Demo | `src/core/control_loop_stage1_demo.cpp` | end-to-end verify |

## Files written at runtime

```text
state/
  checkpoint.json      # durable state + content_hash
  checkpoint.json.bak  # previous checkpoint
  checkpoint.json.tmp  # atomic write temp
  audit.jsonl          # append-only event log
```

## How to enable

Stage1 is **off by default** so existing demos stay unchanged.

```cpp
ControlLoop loop(oracle, strategy);
loop.initialize_from_hardware();  // sets cost scale; does not clobber Stage1 budget after init

std::string err;
if (!loop.initialize_stage1(err)) {
    // integrity/load failure → safe mode
}

auto result = loop.run_cycle(code_snippet);
```

## Cycle flow (Stage1 on)

```text
predict
  → precheck (audit / recovery / budget)
  → audit precheck event
  → spend energy
  → strategy claim
  → oracle truth
  → compare claim vs truth
  → refund/penalty + self-model update
  → checkpoint + audit postcheck
```

On gate failure (no energy / not legible): audit + persist skip/block counters, no learning update.

## Build & run demo

```bash
cd fungal
mkdir -p build && cd build
cmake ..
make -j4 fungal_stage1_demo
./fungal_stage1_demo
```

Expected artifacts after a successful run:
- `state/checkpoint.json` exists
- `state/audit.jsonl` has lines
- reload path in demo restores budget / counters / checkpoint_seq

## Dependencies

- CMake ≥ 3.14 (FetchContent)
- nlohmann/json (fetched by CMake)
- C++17 filesystem

## Honesty notes

- Stage1 is durable state for a **single process lineage**, not a daemon supervisor.
- Strategy heuristics remain weak by design; oracle quality determines learning honesty.
- Production modules under `include/production/` are a separate shell and are not required for Stage1.
