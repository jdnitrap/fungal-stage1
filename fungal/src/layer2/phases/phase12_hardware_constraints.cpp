#include "phase12_hardware_constraints.hpp"

namespace fungal {

void Phase12HardwareConstraints::execute(FungalNetwork& network) {
    if (static_cast<int>(network.nodes.size()) > MAX_NODES_HARD_LIMIT) {
        int excess = network.nodes.size() - MAX_NODES_HARD_LIMIT;
        while (excess > 0 && !network.nodes.empty()) {
            auto it = network.nodes.begin();
            while (it != network.nodes.end() && excess > 0) {
                if (it->energy.available < MIN_ENERGY_THRESHOLD) {
                    it = network.nodes.erase(it);
                    excess--;
                } else {
                    ++it;
                }
            }
            if (excess > 0) break;
        }
    }
}

}  // namespace fungal
