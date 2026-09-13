#include "phase11_measurement_uncertainty.hpp"
#include <algorithm>
#include <cmath>

namespace fungal {

void Phase11MeasurementUncertainty::execute(FungalNetwork& network) {
    double uncertainty_factor = 0.05 * (network.substrate.toxin_level +
                                        std::abs(network.substrate.nutrient_level - 1.0));

    for (auto& claim : network.claims) {
        double adjusted_confidence = claim.confidence * (1.0 - uncertainty_factor);
        claim.confidence = std::max(0.1, adjusted_confidence);
    }
}

}  // namespace fungal
