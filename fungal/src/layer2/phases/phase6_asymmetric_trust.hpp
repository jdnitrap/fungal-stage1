#pragma once

#include "interfaces.hpp"

namespace fungal {

class Phase6AsymmetricTrust : public Phase {
public:
    void execute(FungalNetwork& network) override;
    std::string name() const override { return "Phase 6: Asymmetric Trust"; }
};

}  // namespace fungal
