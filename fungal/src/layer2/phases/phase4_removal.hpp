#pragma once

#include "interfaces.hpp"

namespace fungal {

class Phase4Removal : public Phase {
public:
    void execute(FungalNetwork& network) override;
    std::string name() const override { return "Phase 4: Removal"; }

private:
    static constexpr int QUARANTINE_DURATION = 10;
};

}  // namespace fungal
