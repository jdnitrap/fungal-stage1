#pragma once

#include "interfaces.hpp"

namespace fungal {

class Phase5EnvironmentalShocks : public Phase {
public:
    void execute(FungalNetwork& network) override;
    std::string name() const override { return "Phase 5: Environmental Shocks"; }
};

}  // namespace fungal
