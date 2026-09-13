#include "voting.hpp"

namespace fungal {

bool VotingSystem::consensus_reached(const std::vector<Node>& nodes, const Claim& claim) const {
    int supporting_votes = 0;
    int total_active = 0;

    for (const auto& node : nodes) {
        if (!node.can_execute()) continue;

        total_active++;
        if (node.claim_id == claim.id) {
            supporting_votes++;
        }
    }

    if (total_active == 0) return false;

    double consensus_threshold = 0.66;
    return (static_cast<double>(supporting_votes) / total_active) > consensus_threshold;
}

double VotingSystem::calculate_consensus_strength(const std::vector<Node>& nodes, const Claim& claim) const {
    int supporting_votes = 0;
    int total_active = 0;

    for (const auto& node : nodes) {
        if (!node.can_execute()) continue;

        total_active++;
        if (node.claim_id == claim.id) {
            supporting_votes += static_cast<int>(node.specialization_score * 10);
        }
    }

    if (total_active == 0) return 0.0;
    return static_cast<double>(supporting_votes) / (total_active * 10);
}

void VotingSystem::update_node_votes(std::vector<Node>& nodes, const std::vector<Claim>& claims) {
    for (auto& node : nodes) {
        for (const auto& claim : claims) {
            if (node.claim_id == claim.id && claim.is_verified()) {
                node.energy.received += claim.confidence * 0.05;
                break;
            }
        }
    }
}

}  // namespace fungal
