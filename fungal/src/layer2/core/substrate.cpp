#include "substrate.hpp"
#include <algorithm>
#include <cmath>

namespace fungal {

void SubstrateManager::initialize_substrate(Substrate& substrate) {
    substrate.growth_rate = 0.05;
    substrate.decay_rate = 0.02;
    substrate.toxin_level = 0.0;
    substrate.nutrient_level = 1.0;
    substrate.cycle_count = 0;
}

void SubstrateManager::step_substrate(Substrate& substrate) {
    substrate.nutrient_level = std::max(0.0, substrate.nutrient_level * (1.0 - substrate.decay_rate));
    substrate.toxin_level = std::max(0.0, substrate.toxin_level * substrate.decay_rate);
    substrate.cycle_count++;
}

void SubstrateManager::add_toxin(Substrate& substrate, double amount) {
    substrate.toxin_level = std::min(1.0, substrate.toxin_level + amount);
}

void SubstrateManager::add_nutrient(Substrate& substrate, double amount) {
    substrate.nutrient_level = std::min(2.0, substrate.nutrient_level + amount);
}

void SubstrateManager::environmental_shock(Substrate& substrate, bool is_toxin_event, double intensity) {
    intensity = std::max(0.0, std::min(1.0, intensity));
    if (is_toxin_event) {
        add_toxin(substrate, intensity);
    } else {
        add_nutrient(substrate, intensity);
    }
}

double SubstrateManager::get_growth_potential(const Substrate& substrate) const {
    double toxin_penalty = substrate.toxin_level * 0.5;
    double nutrient_bonus = substrate.nutrient_level * substrate.growth_rate;
    return std::max(0.0, nutrient_bonus - toxin_penalty);
}

}  // namespace fungal
