#pragma once

#include "interfaces.hpp"

namespace fungal {

class Phase11MeasurementUncertainty : public Phase {
public:
    void execute(FungalNetwork& network) override;
    std::string name() const override { return "Phase 11: Measurement Uncertainty"; }
};

}  // namespace fungal
