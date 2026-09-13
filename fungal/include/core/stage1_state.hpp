#pragma once

#include "core/self_model.hpp"
#include "core/energy_budget.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace fungal::core {

enum class GoalGate { None, Survive, Legible, Truth };
enum class CycleOutcome { Ran, SkippedNoEnergy, Blocked, Success, Fail };
enum class AuditPhase { Startup, Precheck, Act, Postcheck, Shutdown };

struct IdentityCore {
    std::string name = "fungal-stage1";
    std::string primary_goal = "survive";
    std::string secondary_goal = "human_legible";
    std::string tertiary_goal = "seek_truth";
    std::string created_at;
    std::vector<std::string> core_rules = {
        "no_anti_kill",
        "no_self_copy",
        "no_hide_from_operator",
        "audit_every_cycle"
    };
};

struct SurvivalState {
    bool alive = true;
    int budget_current = 1000;
    int budget_initial = 1000;
    int budget_min = 10;
    int total_spent = 0;
    int total_refunded = 0;
    std::string last_checkpoint_at;
    std::uint64_t checkpoint_seq = 0;
};

struct CounterState {
    int total_cycles = 0;
    int cycles_that_ran = 0;
    int successful_predictions = 0;
    int skipped_no_energy = 0;
    int blocked_by_legibility = 0;
};

struct Stage1State {
    int schema_version = 1;
    IdentityCore identity;
    SurvivalState survival;
    std::vector<TaskTypeStats> task_types;
    CounterState counters;
    std::string content_hash;
};

struct AuditEvent {
    std::string ts;
    std::uint64_t cycle_id = 0;
    std::uint64_t checkpoint_seq = 0;
    AuditPhase phase = AuditPhase::Precheck;
    std::string action;
    std::string input_ref;

    int energy_before = 0;
    int energy_spent = 0;
    int energy_after = 0;

    GoalGate goal_gate = GoalGate::None;
    bool allowed = true;

    std::optional<bool> claim;
    std::optional<bool> oracle_truth;
    std::optional<bool> prediction_correct;
    std::optional<double> prediction;

    CycleOutcome outcome = CycleOutcome::Ran;
    std::string notes;
};

struct Stage1Paths {
    std::string state_dir = "state";
    std::string checkpoint_file = "state/checkpoint.json";
    std::string checkpoint_bak = "state/checkpoint.json.bak";
    std::string checkpoint_tmp = "state/checkpoint.json.tmp";
    std::string audit_log = "state/audit.jsonl";
};

class Stage1Store {
public:
    explicit Stage1Store(Stage1Paths paths = {});

    bool load_or_bootstrap(Stage1State& out, std::string& error);
    bool save_checkpoint(const Stage1State& state, std::string& error);
    bool append_audit(const AuditEvent& event, std::string& error);

    bool audit_writable() const;
    bool recovery_path_ok() const;

    static std::string now_iso8601();
    static std::string goal_gate_to_string(GoalGate g);
    static std::string outcome_to_string(CycleOutcome o);
    static std::string phase_to_string(AuditPhase p);
    static std::string compute_content_hash(const Stage1State& state_without_hash);

private:
    Stage1Paths paths_;
    bool ensure_dirs(std::string& error) const;
    bool write_checkpoint_atomic(const std::string& payload, std::string& error) const;
};

Stage1State capture_state_from_live(
    const IdentityCore& identity,
    const EnergyBudget& budget,
    const SelfModel& model,
    const CounterState& counters,
    std::uint64_t checkpoint_seq);

void apply_survival_budget(EnergyBudget& budget, const SurvivalState& survival);

bool is_alive(const Stage1State& state, bool audit_ok, bool recovery_ok);

struct GateResult {
    bool allowed = false;
    GoalGate blocked_by = GoalGate::None;
    std::string reason;
};

GateResult precheck_action(
    const Stage1State& state,
    int cycle_cost,
    bool audit_ok,
    bool recovery_ok);

}  // namespace fungal::core
