#include "phase5_environmental_shocks.hpp"
#include <cmath>

namespace fungal {

void Phase5EnvironmentalShocks::execute(FungalNetwork& network) {
    if (network.total_cycles % 20 == 0) {
        bool is_toxin = (network.total_cycles / 20) % 2 == 0;
        double intensity = 0.3;
        network.substrate.toxin_level += is_toxin ? intensity : 0.0;
        network.substrate.nutrient_level += !is_toxin ? intensity : 0.0;
    }
}

}  // namespace fungal
