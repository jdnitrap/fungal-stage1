#pragma once

#include "substrate.hpp"
#include "introspection.hpp"
#include "hardware.hpp"
#include <string>
#include <memory>
#include <vector>
#include <map>

namespace fungal {

struct NodeState {
    std::string node_id;
    double energy;
    double specialization;
    int claim_count;
    std::string current_strategy;
    bool is_learning;
};

class AutonomousNode {
public:
    AutonomousNode(const std::string& node_id, std::shared_ptr<Substrate> substrate);

    void initialize();
    void process_cycle();

    void set_goal(const std::string& goal);
    std::string get_current_goal() const;

    void transfer_energy(double amount);
    void consume_energy(double amount);
    double get_energy() const;

    void read_substrate_signals();
    void broadcast_result(const std::string& result_type, const std::string& content);

    NodeState get_state() const;
    std::string get_id() const;

    void run_introspection();
    IntrospectionMetrics get_self_awareness() const;

private:
    std::string node_id_;
    std::shared_ptr<Substrate> substrate_;
    std::shared_ptr<IntrospectiveLoop> introspection_;

    double energy_;
    double specialization_;
    std::vector<std::string> claims_;
    std::string current_goal_;
    std::string current_strategy_;
    int cycle_count_;

    void update_strategy();
    void process_signals();
    void make_decision();
    void execute_strategy();
};

}  // namespace fungal
