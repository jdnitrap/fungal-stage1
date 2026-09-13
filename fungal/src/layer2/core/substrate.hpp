#pragma once

#include "types.hpp"

namespace fungal {

class SubstrateManager {
public:
    void initialize_substrate(Substrate& substrate);
    void step_substrate(Substrate& substrate);
    void add_toxin(Substrate& substrate, double amount);
    void add_nutrient(Substrate& substrate, double amount);
    void environmental_shock(Substrate& substrate, bool is_toxin_event, double intensity);
    double get_growth_potential(const Substrate& substrate) const;
};

}  // namespace fungal
