#pragma once

#include "interfaces.hpp"

namespace fungal {

class Phase7Specialization : public Phase {
public:
    void execute(FungalNetwork& network) override;
    std::string name() const override { return "Phase 7: Specialization"; }
};

}  // namespace fungal
