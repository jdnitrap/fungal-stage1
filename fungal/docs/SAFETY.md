# Fungal System Safety Mechanisms

Safety in Fungal has two related layers:

1. **Stage1 survival gates** (current research spine)
2. **Segment safety modules** (constraints / alignment / monitor / killswitch)

This document covers both. Stage1 details also live in [STAGE1.md](STAGE1.md).

## Safety Philosophy

1. **Hard limits** beat soft intentions
2. **Legibility** is part of safety (if it cannot be audited, Stage1 will not act)
3. **Scarcity** bounds damage from runaway loops
4. **Operator visibility** is required — Stage1 does not hide, self-copy, or anti-kill
5. **Emergency stop** remains a first-class concept in the broader safety segment

---

## Stage1 safety posture (active spine)

### Goal stack

1. Survive
2. Human-legible
3. Seek truth

Higher goals override lower ones.

### Core rules (Stage1 identity)

- `no_anti_kill`
- `no_self_copy`
- `no_hide_from_operator`
- `audit_every_cycle`

### Gates before action

Stage1 precheck requires:

- audit log writable
- recovery path (`state/`) available
- budget sufficient for the cycle cost
- budget not below configured minimum
- not already in safe mode

If gates fail:

- action is blocked
- event is audited when possible
- skip/block counters are persisted
- learning update does not run

### Safe mode

Entered when checkpoint/audit integrity or write path fails.
While in safe mode, Stage1-enabled `ControlLoop` refuses further actions.

### Energy scarcity

- Spend happens only after precheck (Stage1 path)
- Refund uses the **same spent cost**
- Failure applies extra penalty and clamps budget at ≥ 0
- Exhausted budget stops work without strategy execution

### Integrity

`state/checkpoint.json` includes a content hash over canonical state payload.
Load failure / hash mismatch → initialize fails → safe mode.

### What Stage1 deliberately does **not** do

- Prevent the operator from killing the process
- Copy itself onto other machines
- Hide state from inspection
- Acquire money, accounts, or hardware

Those were explicitly excluded from Stage1 scope.

---

## Segment safety modules (parallel track)

### Hard constraints

`ConstraintEngine` encodes non-negotiable limits such as:

- energy ceilings
- strategy whitelist
- execution timeout ideas

### Value alignment

`ValueAlignment` weights include seek_truth, minimize_harm, respect_constraints, transparency.

### Monitor / audit trail

`Monitor` records decisions and can generate audit reports.

Stage1 audit JSONL is a **separate, core-loop-native** legibility path focused on cycle events.

### Killswitch

Broader system design includes an always-available stop concept for node/system halt.
Stage1 complements this by remaining operator-killable and inspectable.

---

## Constraint enforcement map

| Layer | Safety role |
|---|---|
| Stage1 precheck | Refuse action if not durable/legible/solvent |
| EnergyBudget | Prevent unbounded cycling |
| Oracle feedback | Keep learning grounded |
| Segment ConstraintEngine | Whitelist / hard limits (when used) |
| Segment Monitor/Killswitch | Audit + emergency stop (when used) |

---

## Known limitations

1. **Strategy quality is weak** — safety depends more on gates + oracle than clever claims
2. **Stage1 is not a process supervisor** — OS can still SIGKILL; durability is state, not immortality
3. **Foundation/safety segments are not fully fused** into Stage1 yet
4. **Production security docs** describe an enterprise shell; do not confuse that with Stage1 research guarantees

---

## Deployment practice (research)

1. Run Stage1 demos in a writable working directory
2. Inspect `state/audit.jsonl` after runs
3. Prefer VMs/containers for experiments
4. Do not treat Stage1 as adversarial containment by itself
5. Keep human ability to stop the process

---

## Related docs

- [STAGE1.md](STAGE1.md)
- [ARCHITECTURE.md](ARCHITECTURE.md)
- [BUILDING.md](BUILDING.md)
- [../../talk.md](../../talk.md)
