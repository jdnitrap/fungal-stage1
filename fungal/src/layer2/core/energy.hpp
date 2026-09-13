#pragma once

#include "types.hpp"
#include <vector>

namespace fungal {

class EnergyManager {
public:
    void initialize_energy(Energy& energy, double initial_amount = 1.0);
    void transfer_energy(Node& from, Node& to, double amount);
    void dissipate_energy(Node& node, double amount);
    void recharge_from_substrate(Node& node, const Substrate& substrate);
    double calculate_available_energy(const FungalNetwork& network) const;
    void distribute_energy_by_confidence(std::vector<Node>& nodes, const std::vector<Claim>& claims);
};

}  // namespace fungal
