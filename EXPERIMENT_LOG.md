# Experiment / Verification Log

Dated, honest record of what was actually built, run, and confirmed —
kept separate from `README.md` and `fungal/docs/` (which describe intent
and architecture) so this repo has a durable record even if all other
context is lost.

---

## 2026-09-09 — First-ever compile + run, code-level verification

Source: `jdnitrap/Class`, branch `mycelium/experiments`, first time this
code was actually compiled and executed (previously existed only as
written code + docs).

**Build & run:**
- `fungal_stage1_demo` built with zero errors via CMake (FetchContent
  auto-pulls `nlohmann_json`)
- Ran 6 evaluation cycles
- Wrote a real checkpoint (`state/checkpoint.json`) and audit log
  (`state/audit.jsonl`)
- Correctly reloaded state into a fresh process — budget, counters, and
  checkpoint sequence all restored

**Code-level findings (read control_loop.cpp, self_model.hpp/cpp,
strategy.cpp, test_oracle.cpp directly, not just docs):**

- **No learning actually happens in the executed path.** `SelfModel`
  passively records accuracy/calibration stats, but that output never
  feeds into `Strategy::apply()` — confirmed by reading
  `control_loop.cpp`: `strategy_->apply(code_snippet)` takes no
  self-model input. Self-improvement/alignment/neural-integration
  modules exist as separate files but are explicitly not fused into
  this path (matches the project's own `IMPLEMENTATION_SUMMARY.md`).
- **Checkpoint hash is corruption-detection only, not tamper-evidence.**
  It has a self-computed SHA-256 hash, and the code comment itself says
  it's for "corruption detection." There is no `prevHash`-style linking
  between `audit.jsonl` lines, so it cannot detect a deleted or
  reordered entry the way a real hash-chained audit log would.
- **The "Oracle" is a hardcoded lookup table**, not real program
  analysis: 20 memorized code snippets (10 bug patterns × 2 variants)
  plus a whitespace-tolerant fuzzy string matcher
  (`fungal/src/core/test_oracle.cpp`).
- **The "Strategy" bug predictor only really works for 3 of its 10
  claimed categories**: null-deref, uninitialized-var, off-by-one have
  real heuristic detection; the other 7 always default to "no bug"
  (`fungal/src/core/strategy.cpp`).

**Context:** durable-execution-for-agents (checkpoint/resume) and
tamper-evident audit trails for autonomous agents are both live,
professionalizing industry/standards categories in 2025–2026 (AWS,
Cloudflare, Vercel, Temporal, Inngest for durable execution; IETF draft
`draft-sharif-agent-audit-trail-00` plus FINRA 2026 recordkeeping rules
for audit trails). Fungal's instinct toward both is well-directed — it's
just not yet built to the rigor those standards define.

---

## 2026-09-12 — Repo split-out + fresh build verification

- Split `fungal/` and `fungal_py/` out of `jdnitrap/Class` into this
  standalone repo (`jdnitrap/fungal-stage1`), since `main` and
  `mycelium/experiments` had identical, byte-for-byte copies of both
  directories (last shared commit `d8480c6`, 2026-08-12: "Fix
  ExternalOracle temp_exe + control_loop energy_spent semantics"). The
  `fungal-architecture` branch was an earlier, now-superseded snapshot
  (2026-08-04, ~17.7k lines diverged) and was not carried forward.
- Re-ran a clean `cmake` configure + `make fungal_stage1_demo` build
  from scratch on this machine to confirm the split didn't break
  anything: **builds clean, 100%, zero errors** (CMake's FetchContent
  pulled `nlohmann_json` from GitHub, ~300MB with history — this step
  is the slow part of a from-scratch build, budget several minutes for
  it on first configure).
- Ran the demo twice in a row against the same `state/` directory to
  re-confirm the persistence/reload path:
  - **Run 1** (fresh state): 6 cycles, budget `1000 -> 937`, 5/6
    predictions correct, `checkpoint_seq` ends at 6, audit log has 13
    lines. Reload-into-fresh-object check inside the same run matches:
    budget 937, checkpoint_seq 6, total_cycles 6.
  - **Run 2** (loads Run 1's checkpoint): correctly resumes at budget
    937 / checkpoint_seq 6, runs 6 more cycles, ends at budget 874,
    checkpoint_seq 12, total_cycles 12, audit log grows to 27 lines.
  - **Confirms the durable-checkpoint claim is real**: state genuinely
    survives a process restart and continues counting rather than
    resetting.
  - **Also reconfirms the "no real learning" finding from 2026-09-09**:
    in both runs, the exact same cycle (#6, the `strncpy` snippet)
    fails identically — the model does not get better at that specific
    case across cycles or across a full reload, consistent with
    `SelfModel`'s stats never feeding back into `Strategy::apply()`.
    **This gap is fixed below (2026-09-13 entry) for the 3 patterns
    the strategy can actually detect — the `strncpy` snippet's failure
    is a detection-coverage gap (`strncpy` isn't one of the 3 detected
    patterns), a separate, unfixed issue, not the feedback-loop gap.**

## 2026-09-13 — Fixed: SelfModel now feeds back into Strategy

**The gap:** `SelfModel` tracked per-cycle accuracy/calibration stats,
but `ControlLoop::generate_and_evaluate()` called
`strategy_->apply(code_snippet)` with no self-model input at all —
confirmed by reading the code directly. Whatever the strategy's
confidence was, it never changed based on accumulated experience.

**The fix, scoped to what's real:** rather than claim a vague "wire
them together," this gives each of the 3 real pattern detectors
(null-deref, uninitialized-var, off-by-one) its own SelfModel task ID
(1/2/3 — task ID 0 stays reserved for ControlLoop's separate overall
per-cycle prediction, unchanged). `Strategy::apply()`'s signature now
takes `const SelfModel&`, and `PatternMatcherStrategy::
compute_confidence()` weights each firing pattern's contribution
(previously a flat `0.25` per pattern) by `self_model.get_accuracy(
pattern_id)` — 0.5 (neutral) for an untested pattern type, climbing
toward 1.0 as that pattern's claims are repeatedly confirmed correct,
or falling toward 0.0 as they're repeatedly confirmed wrong.
`ControlLoop::update_pattern_accuracy()` (called every cycle, right
after the existing `commit_and_act()`) is what actually writes back to
SelfModel for whichever pattern(s) fired that cycle.

**Verified two ways, in `tests/unit/test_pattern_accuracy_feedback.cpp`
(new, plain `main()`-based test matching this codebase's existing style
for GTest-optional tests):**

1. **Strategy in isolation:** the identical null-deref-triggering
   snippet, evaluated against three different `SelfModel` states —
   fresh/untested (0.5 accuracy), a simulated 20/20 correct track
   record (accuracy 1.0), and a simulated 0/20 correct track record
   (accuracy 0.0) — produces confidences of 0.396, 0.513, and 0.280
   respectively. Good record > baseline > bad record, strictly, as
   intended.
2. **Full `ControlLoop`, end to end:** starting from a fresh loop
   (null-deref accuracy at the 0.5 default), running 16 real cycles
   against a real `Oracle` on the same null-deref snippet drives that
   pattern's SelfModel accuracy from 0.5 to 1.0, and
   `strategy_confidence` for the identical snippet rises from 0.396 on
   cycle 1 to 0.513 on cycle 17 — a measured behavior change driven
   purely by the loop's own accumulated experience, not a parameter
   tweak.

All 12 checks in the new test pass. Full existing build (`fungal`,
`fungal_tui`, `fungal_sim`, `fungal_stage1_demo`, all `fungal_loop_*`
demos, `real_oracle_validator`) re-verified compiling clean with the
`Strategy::apply()` interface change — only one implementation
(`PatternMatcherStrategy`) and one call site
(`ControlLoop::generate_and_evaluate()`) existed, so the signature
change was safe. `fungal_stage1_demo`'s existing 6-cycle output is
unchanged (same 5/6 correct, same cycle 6 failure) — this fix changes
*confidence weighting* for patterns that already fire, not detection
coverage, so no behavior change was expected or seen on that demo's
existing snippets.

**What this does NOT fix, by design:** the 7 undetected bug categories
still always return "no bug" — that's a detection-coverage gap
(`README.md` item 3), a separate and larger effort from closing the
feedback loop for the 3 categories that already have real detection.
