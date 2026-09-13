# Fungal System Docs

C++17 research system for hardware-aware, energy-constrained, truth-seeking autonomous behavior.

## Quick Start

```bash
cd fungal
mkdir -p build && cd build
cmake ..
make -j4

./fungal_stage1_demo   # Stage1 checkpoint + audit path
./fungal_loop_demo     # core loop without Stage1
./fungal_tui
./fungal_sim
./fungal
```

CMake **3.14+** required (FetchContent for nlohmann/json).

## Read order

1. **[STAGE1.md](STAGE1.md)** — current survival architecture (start here for autonomy work)
2. **[ARCHITECTURE.md](ARCHITECTURE.md)** — full system map
3. **[BUILDING.md](BUILDING.md)** — build, deps, demos
4. **[SAFETY.md](SAFETY.md)** — constraints + Stage1 gates
5. **[../../talk.md](../../talk.md)** — live coordination / decisions

### User guides
- [TUI_GUIDE.md](TUI_GUIDE.md)
- [SIMULATION_GUIDE.md](SIMULATION_GUIDE.md)
- [EXAMPLES.md](EXAMPLES.md)
- [LEARNING.md](LEARNING.md)
- [API.md](API.md)

### Production shell (optional path)
- [PRODUCTION_DEPLOYMENT.md](PRODUCTION_DEPLOYMENT.md)
- [PRODUCTION_API.md](PRODUCTION_API.md)
- [PRODUCTION_CONFIGURATION.md](PRODUCTION_CONFIGURATION.md)
- [PRODUCTION_MONITORING.md](PRODUCTION_MONITORING.md)
- [PRODUCTION_SECURITY.md](PRODUCTION_SECURITY.md)

## What matters right now

The **core control loop** is the runnable spine:

- `EnergyBudget` — scarcity
- `SelfModel` — prediction + calibration
- `Strategy` — claim generation
- `Oracle` — ground truth
- `Stage1Store` — durable checkpoint + audit (opt-in)

Layer1/Layer2 and foundation/safety/learning modules exist in-tree and are partially parallel. Production modules are an enterprise shell, not required for Stage1.

## Stage1 in one paragraph

Stage1 makes the loop durable and inspectable under a fixed goal stack: **survive > human-legible > seek truth**. It writes `state/checkpoint.json` and `state/audit.jsonl`. It does **not** implement anti-kill, self-copy, or concealment.

## Status snapshot

| Piece | Status |
|---|---|
| Core loop | Present |
| Stage1 | On main, opt-in |
| `fungal_stage1_demo` | Target added |
| Local Stage1 proof | Run demo on your machine |
| Full autonomy | Not started |
