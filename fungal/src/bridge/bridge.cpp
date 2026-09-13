#include "bridge.hpp"
#include "../layer2/core/node.hpp"
#include <sstream>

namespace fungal {

void BridgeIntegration::import_foundational_claims(
    FungalNetwork& network,
    const std::vector<Claim>& foundational_claims) {

    for (const auto& claim : foundational_claims) {
        network.claims.push_back(claim);
    }

    create_nodes_for_claims(network, foundational_claims);
}

std::vector<Claim> BridgeIntegration::export_verified_claims(const FungalNetwork& network) {
    std::vector<Claim> verified;

    for (const auto& claim : network.claims) {
        if (claim.is_verified()) {
            verified.push_back(claim);
        }
    }

    return verified;
}

void BridgeIntegration::provide_measurement_feedback(
    const CodeStats& stats,
    std::vector<Claim>& claims) {

    for (auto& claim : claims) {
        if (stats.max_nesting_depth > 5 && claim.statement.find("recursion") != std::string::npos) {
            claim.confidence = std::min(1.0, claim.confidence + 0.1);
            claim.supporting_evidence.push_back("High nesting depth detected");
        }

        if (stats.branch_count > 10 && claim.statement.find("branch") != std::string::npos) {
            claim.confidence = std::min(1.0, claim.confidence + 0.1);
        }
    }
}

void BridgeIntegration::save_network_state(const FungalNetwork& network, const std::string& filename) {
}

FungalNetwork BridgeIntegration::load_network_state(const std::string& filename) {
    return FungalNetwork();
}

void BridgeIntegration::create_nodes_for_claims(
    FungalNetwork& network,
    const std::vector<Claim>& claims) {

    NodeManager nm;
    for (const auto& claim : claims) {
        if (network.can_add_node()) {
            Node node = nm.create_node(claim.id);
            network.nodes.push_back(node);
        }
    }
}

}  // namespace fungal
