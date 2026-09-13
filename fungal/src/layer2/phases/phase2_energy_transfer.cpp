#include "phase2_energy_transfer.hpp"
#include <algorithm>

namespace fungal {

void Phase2EnergyTransfer::execute(FungalNetwork& network) {
    if (network.nodes.size() < 2) return;

    for (size_t i = 0; i < network.nodes.size() - 1; ++i) {
        auto& donor = network.nodes[i];
        if (!donor.can_execute()) continue;

        for (size_t j = i + 1; j < network.nodes.size(); ++j) {
            auto& recipient = network.nodes[j];
            if (!recipient.can_execute()) continue;

            for (const auto& claim : network.claims) {
                if (donor.claim_id == claim.id && recipient.claim_id == claim.id) {
                    double transfer_amount = donor.energy.available * claim.confidence * 0.1;
                    transfer_amount = std::min(transfer_amount, 0.2);
                    donor.energy.available -= transfer_amount;
                    recipient.energy.available += transfer_amount;
                    break;
                }
            }
        }
    }
}

}  // namespace fungal
