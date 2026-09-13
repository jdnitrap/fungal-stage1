#include "../include/fungal_system.hpp"
#include "bridge/bridge.hpp"

namespace fungal {

extern Layer1System* create_layer1_system();
extern Layer2System* create_layer2_system();

FungalSystem::FungalSystem()
    : layer1(create_layer1_system()),
      layer2(create_layer2_system()),
      bridge(std::make_unique<BridgeIntegration>()) {
}

FungalSystem::~FungalSystem() = default;

void FungalSystem::initialize() {
    layer1->initialize();
    layer2->initialize();
}

void FungalSystem::analyze_code(const std::string& code) {
    layer1->analyze_code(code);
    auto foundational_claims = layer1->get_foundational_claims();
    bridge->import_foundational_claims(layer2->get_network(), foundational_claims);
}

void FungalSystem::run_reasoning_cycle(int num_phases) {
    layer2->run_cycle(num_phases);
}

void FungalSystem::apply_measurement_feedback(const CodeStats& stats) {
    auto& network = layer2->get_network();
    bridge->provide_measurement_feedback(stats, network.claims);
}

std::vector<Claim> FungalSystem::get_verified_claims() const {
    return bridge->export_verified_claims(layer2->get_network());
}

FungalNetwork& FungalSystem::get_network() {
    return layer2->get_network();
}

int FungalSystem::get_cycle_count() const {
    return layer2->get_network().total_cycles;
}

int FungalSystem::get_active_node_count() const {
    return layer2->get_network().active_node_count();
}

double FungalSystem::get_average_claim_confidence() const {
    const auto& claims = layer2->get_network().claims;
    if (claims.empty()) return 0.0;

    double total = 0.0;
    for (const auto& claim : claims) {
        total += claim.confidence;
    }
    return total / claims.size();
}

}  // namespace fungal
