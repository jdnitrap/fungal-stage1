#pragma once

#include "interfaces.hpp"

namespace fungal {

class Phase9ClaimRefinement : public Phase {
public:
    void execute(FungalNetwork& network) override;
    std::string name() const override { return "Phase 9: Claim Refinement"; }
};

}  // namespace fungal
