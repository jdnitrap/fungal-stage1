#include "node.hpp"
#include <sstream>

namespace fungal {

Node NodeManager::create_node(const std::string& claim_id) {
    Node node;
    std::ostringstream oss;
    oss << "node_" << node_counter++;
    node.id = oss.str();
    node.claim_id = claim_id;
    node.energy.available = 1.0;
    node.specialization_score = 0.0;
    node.is_quarantined = false;
    node.creation_cycle = 0;
    return node;
}

void NodeManager::remove_node(const std::string& node_id, FungalNetwork& network) {
    auto it = network.nodes.begin();
    while (it != network.nodes.end()) {
        if (it->id == node_id) {
            it = network.nodes.erase(it);
        } else {
            ++it;
        }
    }
}

void NodeManager::update_node_energy(Node& node, double energy_delta) {
    node.energy.available = std::max(0.0, node.energy.available + energy_delta);
}

void NodeManager::quarantine_node(Node& node, int current_cycle) {
    node.is_quarantined = true;
    node.quarantine_cycle = current_cycle;
}

void NodeManager::release_node(Node& node) {
    node.is_quarantined = false;
    node.quarantine_cycle = -1;
    node.trust_violations = 0;
}

void NodeManager::increase_specialization(Node& node, int domain, double amount) {
    node.specialization_domain = domain;
    node.specialization_score = std::min(1.0, node.specialization_score + amount);
}

}  // namespace fungal
