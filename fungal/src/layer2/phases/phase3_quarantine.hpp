#pragma once

#include "interfaces.hpp"

namespace fungal {

class Phase3Quarantine : public Phase {
public:
    void execute(FungalNetwork& network) override;
    std::string name() const override { return "Phase 3: Quarantine"; }

private:
    static constexpr int VIOLATION_THRESHOLD = 3;
};

}  // namespace fungal
