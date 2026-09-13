#pragma once

#include "../include/interfaces.hpp"
#include <vector>
#include <string>

namespace fungal {

class BridgeIntegration {
public:
    void import_foundational_claims(
        FungalNetwork& network,
        const std::vector<Claim>& foundational_claims);

    std::vector<Claim> export_verified_claims(const FungalNetwork& network);

    void provide_measurement_feedback(
        const CodeStats& stats,
        std::vector<Claim>& claims);

    void save_network_state(const FungalNetwork& network, const std::string& filename);
    FungalNetwork load_network_state(const std::string& filename);

private:
    void create_nodes_for_claims(FungalNetwork& network, const std::vector<Claim>& claims);
};

}  // namespace fungal
