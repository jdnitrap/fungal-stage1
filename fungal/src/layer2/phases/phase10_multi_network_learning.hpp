#pragma once

#include "interfaces.hpp"

namespace fungal {

class Phase10MultiNetworkLearning : public Phase {
public:
    void execute(FungalNetwork& network) override;
    std::string name() const override { return "Phase 10: Multi-Network Learning"; }
};

}  // namespace fungal
