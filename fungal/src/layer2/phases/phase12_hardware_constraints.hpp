#pragma once

#include "interfaces.hpp"

namespace fungal {

class Phase12HardwareConstraints : public Phase {
public:
    void execute(FungalNetwork& network) override;
    std::string name() const override { return "Phase 12: Hardware Constraints"; }

private:
    static constexpr int MAX_NODES_HARD_LIMIT = 1000;
    static constexpr double MIN_ENERGY_THRESHOLD = 0.1;
};

}  // namespace fungal
