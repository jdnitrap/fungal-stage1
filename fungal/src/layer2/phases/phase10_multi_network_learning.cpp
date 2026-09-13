#include "phase10_multi_network_learning.hpp"
#include <algorithm>

namespace fungal {

void Phase10MultiNetworkLearning::execute(FungalNetwork& network) {
    for (auto& claim : network.claims) {
        if (claim.is_verified() && claim.confidence > 0.8) {
            claim.confidence = std::min(1.0, claim.confidence + 0.05);
            for (auto& other_claim : network.claims) {
                if (other_claim.statement.find(claim.statement.substr(0, 5)) != std::string::npos) {
                    other_claim.confidence = std::min(1.0, other_claim.confidence + 0.02);
                }
            }
        }
    }
}

}  // namespace fungal
