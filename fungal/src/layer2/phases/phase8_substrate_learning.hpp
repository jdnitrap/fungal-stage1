#pragma once

#include "interfaces.hpp"

namespace fungal {

class Phase8SubstrateLearning : public Phase {
public:
    void execute(FungalNetwork& network) override;
    std::string name() const override { return "Phase 8: Substrate Learning"; }
};

}  // namespace fungal
