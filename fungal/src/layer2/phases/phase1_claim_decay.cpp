#include "phase1_claim_decay.hpp"
#include <algorithm>

namespace fungal {

void Phase1ClaimDecay::execute(FungalNetwork& network) {
    for (auto& claim : network.claims) {
        claim.age_in_cycles++;
        claim.confidence = std::max(0.0, claim.confidence * (1.0 - DECAY_RATE));
    }
}

}  // namespace fungal
