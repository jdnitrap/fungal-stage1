#pragma once

#include "interfaces.hpp"

namespace fungal {

class Phase1ClaimDecay : public Phase {
public:
    void execute(FungalNetwork& network) override;
    std::string name() const override { return "Phase 1: Claim Decay"; }

private:
    static constexpr double DECAY_RATE = 0.05;
};

}  // namespace fungal
