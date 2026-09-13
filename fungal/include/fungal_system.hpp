#pragma once

#include "types.hpp"
#include "interfaces.hpp"
#include <memory>
#include <string>
#include <vector>

namespace fungal {

class BridgeIntegration;

class FungalSystem {
public:
    FungalSystem();
    ~FungalSystem();

    void initialize();
    void analyze_code(const std::string& code);
    void run_reasoning_cycle(int num_phases = 12);
    void apply_measurement_feedback(const CodeStats& stats);
    std::vector<Claim> get_verified_claims() const;
    FungalNetwork& get_network();

    int get_cycle_count() const;
    int get_active_node_count() const;
    double get_average_claim_confidence() const;

private:
    std::unique_ptr<Layer1System> layer1;
    std::unique_ptr<Layer2System> layer2;
    std::unique_ptr<BridgeIntegration> bridge;
};

}  // namespace fungal
