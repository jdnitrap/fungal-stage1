#include "energy.hpp"
#include <algorithm>

namespace fungal {

void EnergyManager::initialize_energy(Energy& energy, double initial_amount) {
    energy.available = initial_amount;
    energy.transferred = 0.0;
    energy.received = 0.0;
    energy.total_dissipated = 0.0;
}

void EnergyManager::transfer_energy(Node& from, Node& to, double amount) {
    double actual_transfer = std::min(amount, from.energy.available);
    from.energy.available -= actual_transfer;
    from.energy.transferred += actual_transfer;
    to.energy.received += actual_transfer;
}

void EnergyManager::dissipate_energy(Node& node, double amount) {
    double actual_dissipation = std::min(amount, node.energy.available);
    node.energy.available -= actual_dissipation;
    node.energy.total_dissipated += actual_dissipation;
}

void EnergyManager::recharge_from_substrate(Node& node, const Substrate& substrate) {
    double recharge_amount = substrate.nutrient_level * 0.1;
    node.energy.available = std::min(1.0, node.energy.available + recharge_amount);
}

double EnergyManager::calculate_available_energy(const FungalNetwork& network) const {
    double total = 0.0;
    for (const auto& node : network.nodes) {
        if (node.can_execute()) {
            total += node.energy.current_level();
        }
    }
    return total;
}

void EnergyManager::distribute_energy_by_confidence(
    std::vector<Node>& nodes,
    const std::vector<Claim>& claims) {

    if (nodes.empty() || claims.empty()) return;

    for (auto& node : nodes) {
        for (const auto& claim : claims) {
            if (node.claim_id == claim.id) {
                double confidence_transfer = claim.confidence * 0.1;
                node.energy.available = std::min(1.0, node.energy.available + confidence_transfer);
                break;
            }
        }
    }
}

}  // namespace fungal
