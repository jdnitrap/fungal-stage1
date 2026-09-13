# Fungal Stage1 — Self-Aware Autonomous Agent (C++ + Python)

A research system exploring **durable, hardware-aware, truth-seeking autonomous
process behavior**, inspired by how fungal mycelial networks sense, allocate
scarce resources, and persist across disruption. This is the "control loop"
family of the fungal project — distinct from the separate
[fungal-llm-network](https://github.com/jdnitrap/fungal-llm-network) distributed
code-generation project, which shares the fungal/mycelium metaphor but is an
unrelated codebase.

Split out from `jdnitrap/Class` (branches `main` / `mycelium/experiments`,
where this code was identical) on 2026-09-12 so it can stand on its own with
its own history and documentation.

## What it is today (Stage1)

A C++17 control loop — `sense → predict → spend → strategy → oracle → learn`
— wrapped in a **survival state machine**:

- **Goal stack** (higher overrides lower): `survive` > `human_legible` > `seek_truth`
- **EnergyBudget**: a real scarcity constraint gating whether the loop can act at all
- **SelfModel**: tracks prediction accuracy/calibration over time
- **Strategy**: a bug-prediction heuristic evaluated against snippets
- **Oracle**: ground-truth feedback used to refund/penalize the self-model
- **Stage1Store**: atomic checkpoint (`state/checkpoint.json`) + append-only
  audit log (`state/audit.jsonl`), so a process can crash and resume with
  intact state
- **Explicit non-goals, enforced in code**: no anti-kill behavior, no
  self-copy, no hiding from the operator, audit every cycle

Two parallel implementations exist and were kept in sync as of the last
commit touching either (2026-08-12):
- `fungal/` — the primary C++ implementation, this is the one with real
  depth (see `fungal/docs/` for the original architecture docs: STAGE1.md,
  ARCHITECTURE.md, SAFETY.md, LEARNING.md, etc.)
- `fungal_py/` — a lighter Python mirror of the same control-loop concepts
  (`control_loop.py`, `energy_budget.py`, `oracle.py`, `self_model.py`,
  `strategy.py`), useful for quick experiments without a C++ build step

Build/run instructions for the C++ side: see `fungal/IMPLEMENTATION_SUMMARY.md`
and `fungal/docs/STAGE1.md`.

## What it should / might do (direction, not yet built)

The `fungal/` tree already contains a much larger set of aspirational modules
that exist as files but are **not wired into the Stage1 control loop path**:
`self_improvement.hpp`, `alignment.hpp`, `neural_integration.hpp`,
`introspection.hpp`, and a full `production/` shell (API server, cluster
manager, DB, metrics, backup). The project's own docs are honest that these
are a "parallel track," not fused in yet.

The intended arc, per `fungal/IMPLEMENTATION_SUMMARY.md`'s own "Next Steps"
and prior analysis (see `EXPERIMENT_LOG.md`):

1. Make `SelfModel`'s calibration output actually feed back into
   `Strategy::apply()` — right now the loop tracks accuracy but never uses
   it to change behavior. This is the single highest-leverage change to make
   the "learning" claim real rather than passive logging.
2. Hash-chain `audit.jsonl` (each entry links to a hash of the previous one)
   so the audit trail is genuinely tamper-evident, not just
   corruption-detecting. Real prior art: IETF draft
   `draft-sharif-agent-audit-trail-00`.
3. Replace the `Oracle`'s hardcoded 20-snippet lookup table with real static
   analysis for at least a few more of the 10 claimed bug categories (only
   null-deref, uninitialized-var, and off-by-one have real heuristic
   detection today — the other 7 always return "no bug").
4. Only after 1–3: consider fusing the self-improvement/alignment modules
   into the live path, and only under the existing Stage1 non-goal
   constraints (no anti-kill, no self-copy, no hiding).

This is deliberately conservative — Stage1's own docs say it is "not full
independence," and the honest next step is making the current, narrow claims
(learning, tamper-evidence, bug detection) actually true before expanding
scope.

## Track record

See `EXPERIMENT_LOG.md` for dated, verified build/run/test results — kept
separate from the architecture docs above because those describe intent,
and this describes what has actually been run and confirmed.
