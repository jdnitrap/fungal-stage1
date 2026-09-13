#pragma once

#include <string>
#include <vector>
#include <ctime>
#include <cmath>

namespace fungal {

// ============================================================
// CORE DATA STRUCTURES - All types used by Fungal
// ============================================================

struct CodeStats {
    int max_nesting_depth = 0;
    int branch_count = 0;
    int function_count = 0;
    int token_count = 0;
};

enum class VerificationStatus {
    UNVERIFIED,
    CONFIRMED,
    CONTRADICTED
};

struct Claim {
    std::string id;
    std::string statement;
    double confidence = 0.5;
    double energy = 1.0;
    int age_in_cycles = 0;
    VerificationStatus verification_status = VerificationStatus::UNVERIFIED;
    std::vector<std::string> supporting_evidence;
    time_t created_at = std::time(nullptr);
    time_t last_updated_at = std::time(nullptr);

    bool is_verified() const {
        return verification_status == VerificationStatus::CONFIRMED;
    }

    bool is_contradicted() const {
        return verification_status == VerificationStatus::CONTRADICTED;
    }
};

struct Energy {
    double available = 1.0;
    double transferred = 0.0;
    double received = 0.0;
    double total_dissipated = 0.0;

    double current_level() const {
        return available + received - transferred - total_dissipated;
    }
};

struct Substrate {
    double growth_rate = 0.05;
    double decay_rate = 0.02;
    double toxin_level = 0.0;
    double nutrient_level = 1.0;
    int cycle_count = 0;

    void step() {
        cycle_count++;
        nutrient_level = std::max(0.0, nutrient_level * (1.0 - decay_rate));
        toxin_level = std::max(0.0, toxin_level * decay_rate);
    }
};

struct Node {
    std::string id;
    std::string claim_id;
    Energy energy;
    double specialization_score = 0.0;
    bool is_quarantined = false;
    int quarantine_cycle = -1;
    int specialization_domain = -1;
    std::vector<std::string> trusted_sources;
    int trust_violations = 0;
    int creation_cycle = 0;

    bool can_execute() const {
        return !is_quarantined && energy.current_level() > 0.1;
    }
};

struct LearningTemplate {
    std::string topic;
    bool topic_recognized = false;
    bool valid = false;
    std::vector<std::string> concepts;
    std::vector<std::string> prerequisites;
};

struct VerifiedRelationship {
    std::string concept1;
    std::string concept2;
    std::string relationship_type;
    double confidence_score = 0.0;
    VerificationStatus status = VerificationStatus::UNVERIFIED;
};

struct FungalNetwork {
    std::vector<Node> nodes;
    std::vector<Claim> claims;
    Substrate substrate;
    int total_cycles = 0;
    int max_nodes = 1000;

    bool can_add_node() const {
        return static_cast<int>(nodes.size()) < max_nodes;
    }

    int active_node_count() const {
        int count = 0;
        for (const auto& node : nodes) {
            if (node.can_execute()) count++;
        }
        return count;
    }

    void step_substrate() {
        substrate.step();
        total_cycles++;
    }
};

}  // namespace fungal
