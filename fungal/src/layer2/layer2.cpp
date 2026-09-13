#include "../include/interfaces.hpp"
#include "core/node.hpp"
#include "core/substrate.hpp"
#include "core/energy.hpp"
#include "voting/voting.hpp"
#include "phases/phase1_claim_decay.hpp"
#include "phases/phase2_energy_transfer.hpp"
#include "phases/phase3_quarantine.hpp"
#include "phases/phase4_removal.hpp"
#include "phases/phase5_environmental_shocks.hpp"
#include "phases/phase6_asymmetric_trust.hpp"
#include "phases/phase7_specialization.hpp"
#include "phases/phase8_substrate_learning.hpp"
#include "phases/phase9_claim_refinement.hpp"
#include "phases/phase10_multi_network_learning.hpp"
#include "phases/phase11_measurement_uncertainty.hpp"
#include "phases/phase12_hardware_constraints.hpp"
#include <vector>
#include <memory>

namespace fungal {

class Layer2SystemImpl : public Layer2System {
private:
    FungalNetwork network;
    std::vector<std::unique_ptr<Phase>> phases;
    VotingSystem voting;

public:
    Layer2SystemImpl() {
        phases.push_back(std::make_unique<Phase1ClaimDecay>());
        phases.push_back(std::make_unique<Phase2EnergyTransfer>());
        phases.push_back(std::make_unique<Phase3Quarantine>());
        phases.push_back(std::make_unique<Phase4Removal>());
        phases.push_back(std::make_unique<Phase5EnvironmentalShocks>());
        phases.push_back(std::make_unique<Phase6AsymmetricTrust>());
        phases.push_back(std::make_unique<Phase7Specialization>());
        phases.push_back(std::make_unique<Phase8SubstrateLearning>());
        phases.push_back(std::make_unique<Phase9ClaimRefinement>());
        phases.push_back(std::make_unique<Phase10MultiNetworkLearning>());
        phases.push_back(std::make_unique<Phase11MeasurementUncertainty>());
        phases.push_back(std::make_unique<Phase12HardwareConstraints>());
    }

    void initialize() override {
        network.nodes.clear();
        network.claims.clear();
        network.total_cycles = 0;
        SubstrateManager sm;
        sm.initialize_substrate(network.substrate);
    }

    void run_cycle(int num_phases) override {
        int phases_to_run = (num_phases > 0 && num_phases <= 12) ? num_phases : 12;

        for (int i = 0; i < phases_to_run; ++i) {
            phases[i]->execute(network);
        }

        network.step_substrate();
    }

    FungalNetwork& get_network() override {
        return network;
    }
};

Layer2System* create_layer2_system() {
    return new Layer2SystemImpl();
}

}  // namespace fungal
