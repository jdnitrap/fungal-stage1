#pragma once

#include "types.hpp"
#include <vector>

namespace fungal {

class VotingSystem {
public:
    bool consensus_reached(const std::vector<Node>& nodes, const Claim& claim) const;
    double calculate_consensus_strength(const std::vector<Node>& nodes, const Claim& claim) const;
    void update_node_votes(std::vector<Node>& nodes, const std::vector<Claim>& claims);
};

}  // namespace fungal
