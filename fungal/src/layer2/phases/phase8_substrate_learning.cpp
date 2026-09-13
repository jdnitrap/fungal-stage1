#include "phase8_substrate_learning.hpp"
#include <algorithm>

namespace fungal {

void Phase8SubstrateLearning::execute(FungalNetwork& network) {
    int active_nodes = network.active_node_count();

    if (active_nodes > 50) {
        network.substrate.growth_rate = std::min(0.10, network.substrate.growth_rate + 0.01);
    } else if (active_nodes < 10) {
        network.substrate.growth_rate = std::max(0.02, network.substrate.growth_rate - 0.01);
    }

    if (network.substrate.toxin_level > 0.5) {
        network.substrate.decay_rate = std::min(0.05, network.substrate.decay_rate + 0.005);
    }
}

}  // namespace fungal
