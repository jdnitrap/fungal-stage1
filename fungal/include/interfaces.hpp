#pragma once

#include "types.hpp"
#include <vector>
#include <string>

namespace fungal {

// ============================================================
// BASE INTERFACES - Contracts for all implementations
// ============================================================

class Phase {
public:
    virtual ~Phase() = default;
    virtual void execute(FungalNetwork& network) = 0;
    virtual std::string name() const = 0;
};

class PhaseExecutor {
public:
    virtual ~PhaseExecutor() = default;
    virtual void run_phase_cycle(FungalNetwork& network) = 0;
};

class Layer1System {
public:
    virtual ~Layer1System() = default;
    virtual void initialize() = 0;
    virtual void analyze_code(const std::string& code) = 0;
    virtual std::vector<Claim> get_foundational_claims() const = 0;
};

class Layer2System {
public:
    virtual ~Layer2System() = default;
    virtual void initialize() = 0;
    virtual void run_cycle(int num_phases) = 0;
    virtual FungalNetwork& get_network() = 0;
};

class ClaimExporter {
public:
    virtual ~ClaimExporter() = default;
    virtual std::string export_claims(const std::vector<Claim>& claims) = 0;
};

class ClaimImporter {
public:
    virtual ~ClaimImporter() = default;
    virtual std::vector<Claim> import_claims(const std::string& data) = 0;
};

class MeasurementFeedback {
public:
    virtual ~MeasurementFeedback() = default;
    virtual void provide_feedback(const CodeStats& stats, std::vector<Claim>& claims) = 0;
};

class StatePersistence {
public:
    virtual ~StatePersistence() = default;
    virtual void save_state(const FungalNetwork& network, const std::string& filename) = 0;
    virtual FungalNetwork load_state(const std::string& filename) = 0;
};

}  // namespace fungal
