#include "phase4_removal.hpp"

namespace fungal {

void Phase4Removal::execute(FungalNetwork& network) {
    auto it = network.nodes.begin();
    while (it != network.nodes.end()) {
        if (it->is_quarantined) {
            int quarantine_age = network.total_cycles - it->quarantine_cycle;
            if (quarantine_age > QUARANTINE_DURATION) {
                auto claim_it = network.claims.begin();
                while (claim_it != network.claims.end()) {
                    if (claim_it->id == it->claim_id) {
                        claim_it = network.claims.erase(claim_it);
                    } else {
                        ++claim_it;
                    }
                }
                it = network.nodes.erase(it);
            } else {
                ++it;
            }
        } else {
            ++it;
        }
    }
}

}  // namespace fungal
