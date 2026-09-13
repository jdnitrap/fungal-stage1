#include "phase7_specialization.hpp"
#include <algorithm>
#include <functional>

namespace fungal {

void Phase7Specialization::execute(FungalNetwork& network) {
    for (auto& node : network.nodes) {
        if (!node.can_execute()) continue;

        for (const auto& claim : network.claims) {
            if (node.claim_id == claim.id && claim.is_verified()) {
                node.specialization_score = std::min(1.0, node.specialization_score + 0.05);
                node.specialization_domain = std::hash<std::string>{}(claim.statement) % 5;
                break;
            }
        }
    }
}

}  // namespace fungal
