#include "phase9_claim_refinement.hpp"
#include <algorithm>

namespace fungal {

void Phase9ClaimRefinement::execute(FungalNetwork& network) {
    for (auto& claim : network.claims) {
        int supporting_node_count = 0;
        for (const auto& node : network.nodes) {
            if (node.claim_id == claim.id && node.can_execute()) {
                supporting_node_count++;
            }
        }

        if (supporting_node_count > 5) {
            claim.confidence = std::min(1.0, claim.confidence + 0.1);
            claim.verification_status = VerificationStatus::CONFIRMED;
        } else if (supporting_node_count == 0) {
            claim.confidence = std::max(0.0, claim.confidence - 0.15);
            claim.verification_status = VerificationStatus::CONTRADICTED;
        }
    }
}

}  // namespace fungal
