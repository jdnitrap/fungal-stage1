#include "autonomous_node.hpp"
#include <iostream>
#include <cmath>

namespace fungal {

AutonomousNode::AutonomousNode(const std::string& node_id, std::shared_ptr<Substrate> substrate)
    : node_id_(node_id), substrate_(substrate), energy_(100.0), specialization_(0.5),
      cycle_count_(0), current_goal_("seek_truth"), current_strategy_("verify_claims") {

    introspection_ = std::make_shared<IntrospectiveLoop>(node_id_);
}

void AutonomousNode::initialize() {
    substrate_->register_node(node_id_);
    substrate_->set_condition("node_active_" + node_id_, 1.0);
}

void AutonomousNode::process_cycle() {
    cycle_count_++;

    read_substrate_signals();
    run_introspection();
    make_decision();
    execute_strategy();

    energy_ -= 1.0;
    if (energy_ < 0) energy_ = 0;
}

void AutonomousNode::set_goal(const std::string& goal) {
    current_goal_ = goal;
}

std::string AutonomousNode::get_current_goal() const {
    return current_goal_;
}

void AutonomousNode::transfer_energy(double amount) {
    energy_ += amount;
    if (energy_ > 1000.0) energy_ = 1000.0;
}

void AutonomousNode::consume_energy(double amount) {
    energy_ -= amount;
    if (energy_ < 0) energy_ = 0;
}

double AutonomousNode::get_energy() const {
    return energy_;
}

void AutonomousNode::read_substrate_signals() {
    auto signals = substrate_->read_signals(node_id_, 10);

    for (const auto& signal : signals) {
        introspection_->observe_reasoning("read_signal", 0.8);
    }
}

void AutonomousNode::broadcast_result(const std::string& result_type, const std::string& content) {
    Signal result{
        result_type,
        node_id_,
        content,
        0.0,
        0.5
    };
    substrate_->broadcast_signal(result);
}

NodeState AutonomousNode::get_state() const {
    return {
        node_id_,
        energy_,
        specialization_,
        static_cast<int>(claims_.size()),
        current_strategy_,
        true
    };
}

std::string AutonomousNode::get_id() const {
    return node_id_;
}

void AutonomousNode::run_introspection() {
    auto metrics = introspection_->get_current_metrics();

    if (introspection_->should_change_strategy()) {
        update_strategy();
    }

    introspection_->assess_resource_usage(1.0, energy_);
}

void AutonomousNode::process_signals() {
    auto signals = substrate_->read_signals(node_id_, 5);

    for (const auto& signal : signals) {
        if (signal.type == "claim_verification") {
            introspection_->evaluate_process("verify_claim", true);
        }
    }
}

void AutonomousNode::make_decision() {
    if (energy_ < 10.0) {
        current_strategy_ = "conserve_energy";
    } else if (introspection_->should_change_strategy()) {
        current_strategy_ = introspection_->recommend_strategy_change();
    }
}

void AutonomousNode::execute_strategy() {
    if (current_strategy_ == "verify_claims") {
        introspection_->observe_reasoning("verify", 0.75);
    } else if (current_strategy_ == "conserve_energy") {
        energy_ += 0.5;
    } else if (current_strategy_ == "increase_verification_redundancy") {
        introspection_->observe_reasoning("redundant_verify", 0.85);
    }

    broadcast_result("cycle_complete", "cycle_" + std::to_string(cycle_count_));
}

void AutonomousNode::update_strategy() {
    std::string recommendation = introspection_->recommend_strategy_change();
    current_strategy_ = recommendation;
}

IntrospectionMetrics AutonomousNode::get_self_awareness() const {
    return introspection_->get_current_metrics();
}

}  // namespace fungal
