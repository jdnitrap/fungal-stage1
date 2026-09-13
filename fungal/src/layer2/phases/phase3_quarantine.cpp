#include "phase3_quarantine.hpp"

namespace fungal {

void Phase3Quarantine::execute(FungalNetwork& network) {
    for (auto& node : network.nodes) {
        if (node.trust_violations >= VIOLATION_THRESHOLD) {
            node.is_quarantined = true;
            node.quarantine_cycle = network.total_cycles;
        }
    }
}

}  // namespace fungal
