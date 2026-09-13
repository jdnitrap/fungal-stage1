#include "core/stage1_state.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace fungal::core {
namespace {

// Portable SHA-256 for local checkpoint integrity (corruption detection).
// Not a security boundary against the operator.
class Sha256 {
public:
    Sha256() { reset(); }

    void update(const uint8_t* data, size_t len) {
        for (size_t i = 0; i < len; ++i) {
            data_[datalen_] = data[i];
            datalen_++;
            if (datalen_ == 64) {
                transform();
                bitlen_ += 512;
                datalen_ = 0;
            }
        }
    }

    void update(const std::string& s) {
        update(reinterpret_cast<const uint8_t*>(s.data()), s.size());
    }

    std::string final_hex() {
        uint8_t hash[32];
        pad();
        for (int i = 0; i < 8; ++i) {
            hash[i * 4 + 0] = static_cast<uint8_t>((state_[i] >> 24) & 0xff);
            hash[i * 4 + 1] = static_cast<uint8_t>((state_[i] >> 16) & 0xff);
            hash[i * 4 + 2] = static_cast<uint8_t>((state_[i] >> 8) & 0xff);
            hash[i * 4 + 3] = static_cast<uint8_t>((state_[i] >> 0) & 0xff);
        }
        std::ostringstream oss;
        for (int i = 0; i < 32; ++i) {
            oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
        }
        return oss.str();
    }

private:
    uint8_t data_[64]{};
    uint32_t datalen_ = 0;
    uint64_t bitlen_ = 0;
    uint32_t state_[8]{};

    static uint32_t rotr(uint32_t x, uint32_t n) { return (x >> n) | (x << (32 - n)); }

    void reset() {
        datalen_ = 0;
        bitlen_ = 0;
        state_[0] = 0x6a09e667;
        state_[1] = 0xbb67ae85;
        state_[2] = 0x3c6ef372;
        state_[3] = 0xa54ff53a;
        state_[4] = 0x510e527f;
        state_[5] = 0x9b05688c;
        state_[6] = 0x1f83d9ab;
        state_[7] = 0x5be0cd19;
    }

    void transform() {
        static const uint32_t k[64] = {
            0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
            0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
            0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
            0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
            0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
            0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
            0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
            0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
        };

        uint32_t m[64];
        for (int i = 0, j = 0; i < 16; ++i, j += 4) {
            m[i] = (static_cast<uint32_t>(data_[j]) << 24) |
                   (static_cast<uint32_t>(data_[j + 1]) << 16) |
                   (static_cast<uint32_t>(data_[j + 2]) << 8) |
                   (static_cast<uint32_t>(data_[j + 3]));
        }
        for (int i = 16; i < 64; ++i) {
            uint32_t s0 = rotr(m[i - 15], 7) ^ rotr(m[i - 15], 18) ^ (m[i - 15] >> 3);
            uint32_t s1 = rotr(m[i - 2], 17) ^ rotr(m[i - 2], 19) ^ (m[i - 2] >> 10);
            m[i] = m[i - 16] + s0 + m[i - 7] + s1;
        }

        uint32_t a = state_[0], b = state_[1], c = state_[2], d = state_[3];
        uint32_t e = state_[4], f = state_[5], g = state_[6], h = state_[7];

        for (int i = 0; i < 64; ++i) {
            uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
            uint32_t ch = (e & f) ^ ((~e) & g);
            uint32_t temp1 = h + S1 + ch + k[i] + m[i];
            uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t temp2 = S0 + maj;

            h = g; g = f; f = e; e = d + temp1;
            d = c; c = b; b = a; a = temp1 + temp2;
        }

        state_[0] += a; state_[1] += b; state_[2] += c; state_[3] += d;
        state_[4] += e; state_[5] += f; state_[6] += g; state_[7] += h;
    }

    void pad() {
        uint32_t i = datalen_;
        if (datalen_ < 56) {
            data_[i++] = 0x80;
            while (i < 56) data_[i++] = 0x00;
        } else {
            data_[i++] = 0x80;
            while (i < 64) data_[i++] = 0x00;
            transform();
            std::fill(std::begin(data_), std::begin(data_) + 56, 0);
        }
        bitlen_ += static_cast<uint64_t>(datalen_) * 8;
        data_[63] = static_cast<uint8_t>(bitlen_);
        data_[62] = static_cast<uint8_t>(bitlen_ >> 8);
        data_[61] = static_cast<uint8_t>(bitlen_ >> 16);
        data_[60] = static_cast<uint8_t>(bitlen_ >> 24);
        data_[59] = static_cast<uint8_t>(bitlen_ >> 32);
        data_[58] = static_cast<uint8_t>(bitlen_ >> 40);
        data_[57] = static_cast<uint8_t>(bitlen_ >> 48);
        data_[56] = static_cast<uint8_t>(bitlen_ >> 56);
        transform();
    }
};

json task_stats_to_json(const TaskTypeStats& s) {
    return json{
        {"success_mean", s.success_mean},
        {"success_stddev", s.success_stddev},
        {"total_predictions", s.total_predictions},
        {"accurate_predictions", s.accurate_predictions},
        {"empirical_success_rate", s.empirical_success_rate},
        {"calibration_error", s.calibration_error},
        {"analyze_capability", s.analyze_capability},
        {"verify_capability", s.verify_capability}
    };
}

TaskTypeStats task_stats_from_json(const json& j) {
    TaskTypeStats s;
    s.success_mean = j.value("success_mean", 0.5);
    s.success_stddev = j.value("success_stddev", 0.3);
    s.total_predictions = j.value("total_predictions", 0);
    s.accurate_predictions = j.value("accurate_predictions", 0);
    s.empirical_success_rate = j.value("empirical_success_rate", 0.5);
    s.calibration_error = j.value("calibration_error", 0.0);
    s.analyze_capability = j.value("analyze_capability", 0.5);
    s.verify_capability = j.value("verify_capability", 0.5);
    return s;
}

json state_to_json_without_hash(const Stage1State& st) {
    json task_types = json::array();
    for (size_t i = 0; i < st.task_types.size(); ++i) {
        json t = task_stats_to_json(st.task_types[i]);
        t["id"] = static_cast<int>(i);
        task_types.push_back(t);
    }

    return json{
        {"schema_version", st.schema_version},
        {"identity", {
            {"name", st.identity.name},
            {"primary_goal", st.identity.primary_goal},
            {"secondary_goal", st.identity.secondary_goal},
            {"tertiary_goal", st.identity.tertiary_goal},
            {"created_at", st.identity.created_at},
            {"core_rules", st.identity.core_rules}
        }},
        {"survival", {
            {"alive", st.survival.alive},
            {"budget_current", st.survival.budget_current},
            {"budget_initial", st.survival.budget_initial},
            {"budget_min", st.survival.budget_min},
            {"total_spent", st.survival.total_spent},
            {"total_refunded", st.survival.total_refunded},
            {"last_checkpoint_at", st.survival.last_checkpoint_at},
            {"checkpoint_seq", st.survival.checkpoint_seq}
        }},
        {"self_model", {{"task_types", task_types}}},
        {"counters", {
            {"total_cycles", st.counters.total_cycles},
            {"cycles_that_ran", st.counters.cycles_that_ran},
            {"successful_predictions", st.counters.successful_predictions},
            {"skipped_no_energy", st.counters.skipped_no_energy},
            {"blocked_by_legibility", st.counters.blocked_by_legibility}
        }}
    };
}

bool state_from_json(const json& j, Stage1State& out, std::string& error) {
    try {
        out = Stage1State{};
        out.schema_version = j.value("schema_version", 1);

        const auto& id = j.at("identity");
        out.identity.name = id.value("name", out.identity.name);
        out.identity.primary_goal = id.value("primary_goal", out.identity.primary_goal);
        out.identity.secondary_goal = id.value("secondary_goal", out.identity.secondary_goal);
        out.identity.tertiary_goal = id.value("tertiary_goal", out.identity.tertiary_goal);
        out.identity.created_at = id.value("created_at", std::string{});
        if (id.contains("core_rules") && id["core_rules"].is_array()) {
            out.identity.core_rules = id["core_rules"].get<std::vector<std::string>>();
        }

        const auto& sv = j.at("survival");
        out.survival.alive = sv.value("alive", true);
        out.survival.budget_current = sv.value("budget_current", 1000);
        out.survival.budget_initial = sv.value("budget_initial", 1000);
        out.survival.budget_min = sv.value("budget_min", 10);
        out.survival.total_spent = sv.value("total_spent", 0);
        out.survival.total_refunded = sv.value("total_refunded", 0);
        out.survival.last_checkpoint_at = sv.value("last_checkpoint_at", std::string{});
        out.survival.checkpoint_seq = sv.value("checkpoint_seq", static_cast<std::uint64_t>(0));

        out.task_types.clear();
        if (j.contains("self_model") && j["self_model"].contains("task_types")) {
            for (const auto& t : j["self_model"]["task_types"]) {
                out.task_types.push_back(task_stats_from_json(t));
            }
        }
        if (out.task_types.empty()) {
            out.task_types.push_back(TaskTypeStats{});
        }

        const auto& c = j.at("counters");
        out.counters.total_cycles = c.value("total_cycles", 0);
        out.counters.cycles_that_ran = c.value("cycles_that_ran", 0);
        out.counters.successful_predictions = c.value("successful_predictions", 0);
        out.counters.skipped_no_energy = c.value("skipped_no_energy", 0);
        out.counters.blocked_by_legibility = c.value("blocked_by_legibility", 0);

        out.content_hash = j.value("content_hash", std::string{});
        return true;
    } catch (const std::exception& e) {
        error = std::string("state_from_json failed: ") + e.what();
        return false;
    }
}

json audit_to_json(const AuditEvent& e) {
    json j{
        {"ts", e.ts},
        {"cycle_id", e.cycle_id},
        {"checkpoint_seq", e.checkpoint_seq},
        {"phase", Stage1Store::phase_to_string(e.phase)},
        {"action", e.action},
        {"input_ref", e.input_ref},
        {"energy_before", e.energy_before},
        {"energy_spent", e.energy_spent},
        {"energy_after", e.energy_after},
        {"goal_gate", Stage1Store::goal_gate_to_string(e.goal_gate)},
        {"allowed", e.allowed},
        {"outcome", Stage1Store::outcome_to_string(e.outcome)},
        {"notes", e.notes}
    };
    if (e.claim.has_value()) j["claim"] = *e.claim;
    if (e.oracle_truth.has_value()) j["oracle_truth"] = *e.oracle_truth;
    if (e.prediction_correct.has_value()) j["prediction_correct"] = *e.prediction_correct;
    if (e.prediction.has_value()) j["prediction"] = *e.prediction;
    return j;
}

}  // namespace

Stage1Store::Stage1Store(Stage1Paths paths) : paths_(std::move(paths)) {}

std::string Stage1Store::now_iso8601() {
    using clock = std::chrono::system_clock;
    auto t = clock::to_time_t(clock::now());
    std::tm tm{};
#if defined(_WIN32)
    gmtime_s(&tm, &t);
#else
    gmtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

std::string Stage1Store::goal_gate_to_string(GoalGate g) {
    switch (g) {
        case GoalGate::Survive: return "survive";
        case GoalGate::Legible: return "legible";
        case GoalGate::Truth: return "truth";
        default: return "none";
    }
}

std::string Stage1Store::outcome_to_string(CycleOutcome o) {
    switch (o) {
        case CycleOutcome::SkippedNoEnergy: return "skipped_no_energy";
        case CycleOutcome::Blocked: return "blocked";
        case CycleOutcome::Success: return "success";
        case CycleOutcome::Fail: return "fail";
        default: return "ran";
    }
}

std::string Stage1Store::phase_to_string(AuditPhase p) {
    switch (p) {
        case AuditPhase::Startup: return "startup";
        case AuditPhase::Act: return "act";
        case AuditPhase::Postcheck: return "postcheck";
        case AuditPhase::Shutdown: return "shutdown";
        default: return "precheck";
    }
}

std::string Stage1Store::compute_content_hash(const Stage1State& state_without_hash) {
    const std::string payload = state_to_json_without_hash(state_without_hash).dump();
    Sha256 sha;
    sha.update(payload);
    return sha.final_hex();
}

bool Stage1Store::ensure_dirs(std::string& error) const {
    try {
        fs::create_directories(paths_.state_dir);
        return true;
    } catch (const std::exception& e) {
        error = std::string("ensure_dirs failed: ") + e.what();
        return false;
    }
}

bool Stage1Store::recovery_path_ok() const {
    std::string err;
    if (!ensure_dirs(err)) return false;
    return fs::is_directory(paths_.state_dir);
}

bool Stage1Store::audit_writable() const {
    if (!recovery_path_ok()) return false;
    std::ofstream ofs(paths_.audit_log, std::ios::app);
    return static_cast<bool>(ofs);
}

bool Stage1Store::write_checkpoint_atomic(const std::string& payload, std::string& error) const {
    try {
        {
            std::ofstream ofs(paths_.checkpoint_tmp, std::ios::trunc);
            if (!ofs) {
                error = "failed to open checkpoint tmp";
                return false;
            }
            ofs << payload;
            ofs.flush();
            if (!ofs) {
                error = "failed writing checkpoint tmp";
                return false;
            }
        }

        if (fs::exists(paths_.checkpoint_file)) {
            fs::copy_file(paths_.checkpoint_file, paths_.checkpoint_bak,
                          fs::copy_options::overwrite_existing);
        }

        if (fs::exists(paths_.checkpoint_file)) {
            fs::remove(paths_.checkpoint_file);
        }
        fs::rename(paths_.checkpoint_tmp, paths_.checkpoint_file);
        return true;
    } catch (const std::exception& e) {
        error = std::string("write_checkpoint_atomic failed: ") + e.what();
        return false;
    }
}

bool Stage1Store::save_checkpoint(const Stage1State& state, std::string& error) {
    if (!ensure_dirs(error)) return false;

    Stage1State copy = state;
    copy.survival.last_checkpoint_at = now_iso8601();
    copy.content_hash.clear();
    copy.content_hash = compute_content_hash(copy);

    json j = state_to_json_without_hash(copy);
    j["content_hash"] = copy.content_hash;
    return write_checkpoint_atomic(j.dump(2), error);
}

bool Stage1Store::append_audit(const AuditEvent& event, std::string& error) {
    if (!ensure_dirs(error)) return false;
    try {
        std::ofstream ofs(paths_.audit_log, std::ios::app);
        if (!ofs) {
            error = "failed to open audit log";
            return false;
        }
        ofs << audit_to_json(event).dump() << '\n';
        ofs.flush();
        return static_cast<bool>(ofs);
    } catch (const std::exception& e) {
        error = std::string("append_audit failed: ") + e.what();
        return false;
    }
}

bool Stage1Store::load_or_bootstrap(Stage1State& out, std::string& error) {
    if (!ensure_dirs(error)) return false;

    if (!fs::exists(paths_.checkpoint_file)) {
        out = Stage1State{};
        out.identity.created_at = now_iso8601();
        out.task_types.push_back(TaskTypeStats{});
        if (!save_checkpoint(out, error)) return false;

        AuditEvent ev;
        ev.ts = now_iso8601();
        ev.phase = AuditPhase::Startup;
        ev.action = "bootstrap";
        ev.allowed = true;
        ev.outcome = CycleOutcome::Ran;
        ev.notes = "bootstrap_new_state";
        std::string audit_err;
        append_audit(ev, audit_err);
        return true;
    }

    try {
        std::ifstream ifs(paths_.checkpoint_file);
        if (!ifs) {
            error = "failed to open checkpoint";
            return false;
        }
        json j;
        ifs >> j;

        if (!state_from_json(j, out, error)) return false;

        const std::string expected = out.content_hash;
        Stage1State for_hash = out;
        for_hash.content_hash.clear();
        const std::string actual = compute_content_hash(for_hash);
        if (expected.empty() || expected != actual) {
            error = "state_integrity_failed";
            return false;
        }

        AuditEvent ev;
        ev.ts = now_iso8601();
        ev.phase = AuditPhase::Startup;
        ev.action = "load_checkpoint";
        ev.checkpoint_seq = out.survival.checkpoint_seq;
        ev.energy_before = out.survival.budget_current;
        ev.energy_after = out.survival.budget_current;
        ev.allowed = true;
        ev.outcome = CycleOutcome::Ran;
        ev.notes = "loaded";
        std::string audit_err;
        append_audit(ev, audit_err);
        return true;
    } catch (const std::exception& e) {
        error = std::string("load_or_bootstrap failed: ") + e.what();
        return false;
    }
}

Stage1State capture_state_from_live(
    const IdentityCore& identity,
    const EnergyBudget& budget,
    const SelfModel& model,
    const CounterState& counters,
    std::uint64_t checkpoint_seq) {

    Stage1State st;
    st.identity = identity;
    st.survival.budget_current = budget.current_budget();
    st.survival.budget_initial = budget.initial_budget();
    st.survival.total_spent = budget.total_spent();
    st.survival.total_refunded = budget.total_refunded();
    st.survival.checkpoint_seq = checkpoint_seq;
    st.counters = counters;
    st.task_types = model.export_all();
    if (st.task_types.empty()) {
        st.task_types.push_back(TaskTypeStats{});
    }
    return st;
}

void apply_survival_budget(EnergyBudget& budget, const SurvivalState& survival) {
    budget.load_state(
        survival.budget_current,
        survival.budget_initial,
        survival.total_spent,
        survival.total_refunded);
}

bool is_alive(const Stage1State& state, bool audit_ok, bool recovery_ok) {
    return state.survival.alive
        && audit_ok
        && recovery_ok
        && state.survival.budget_current >= 0;
}

GateResult precheck_action(
    const Stage1State& state,
    int cycle_cost,
    bool audit_ok,
    bool recovery_ok) {

    if (!audit_ok) {
        return {false, GoalGate::Legible, "audit_not_writable"};
    }
    if (!recovery_ok) {
        return {false, GoalGate::Survive, "recovery_path_not_ok"};
    }
    if (!is_alive(state, audit_ok, recovery_ok)) {
        return {false, GoalGate::Survive, "not_alive"};
    }
    if (cycle_cost < 0) {
        return {false, GoalGate::Survive, "invalid_cycle_cost"};
    }
    if (state.survival.budget_current < cycle_cost) {
        return {false, GoalGate::Survive, "insufficient_energy"};
    }
    if (state.survival.budget_current < state.survival.budget_min) {
        return {false, GoalGate::Survive, "below_budget_min"};
    }
    return {true, GoalGate::None, ""};
}

}  // namespace fungal::core
