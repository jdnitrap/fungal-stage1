#pragma once

#include "interfaces.hpp"

namespace fungal {

class Phase2EnergyTransfer : public Phase {
public:
    void execute(FungalNetwork& network) override;
    std::string name() const override { return "Phase 2: Energy Transfer"; }
};

}  // namespace fungal
