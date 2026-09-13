#include "phase6_asymmetric_trust.hpp"

namespace fungal {

void Phase6AsymmetricTrust::execute(FungalNetwork& network) {
    for (auto& node : network.nodes) {
        if (!node.trusted_sources.empty()) {
            for (const auto& source : node.trusted_sources) {
                for (auto& other : network.nodes) {
                    if (other.id == source && other.is_quarantined) {
                        node.trust_violations++;
                        break;
                    }
                }
            }
        }
    }
}

}  // namespace fungal
