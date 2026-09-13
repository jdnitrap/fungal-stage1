#include "../../src/layer2/phases/phase1_claim_decay.hpp"
#include "../../src/layer2/phases/phase2_energy_transfer.hpp"
#include "../../include/types.hpp"
#include <gtest/gtest.h>

using namespace fungal;

TEST(PhaseTest, ClaimDecayReducesConfidence) {
    Phase1ClaimDecay phase;
    FungalNetwork network;

    Claim claim;
    claim.id = "test_claim";
    claim.statement = "Test statement";
    claim.confidence = 1.0;
    network.claims.push_back(claim);

    phase.execute(network);

    EXPECT_LT(network.claims[0].confidence, 1.0);
    EXPECT_GT(network.claims[0].confidence, 0.0);
}

TEST(PhaseTest, ClaimAgeIncreases) {
    Phase1ClaimDecay phase;
    FungalNetwork network;

    Claim claim;
    claim.age_in_cycles = 0;
    network.claims.push_back(claim);

    phase.execute(network);

    EXPECT_EQ(network.claims[0].age_in_cycles, 1);
}

TEST(PhaseTest, EnergyTransferWithoutNodes) {
    Phase2EnergyTransfer phase;
    FungalNetwork network;

    phase.execute(network);

    EXPECT_EQ(network.nodes.size(), 0);
}
