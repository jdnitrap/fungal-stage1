#pragma once

#include "types.hpp"

namespace fungal {

class NodeManager {
public:
    Node create_node(const std::string& claim_id);
    void remove_node(const std::string& node_id, FungalNetwork& network);
    void update_node_energy(Node& node, double energy_delta);
    void quarantine_node(Node& node, int current_cycle);
    void release_node(Node& node);
    void increase_specialization(Node& node, int domain, double amount);

private:
    int node_counter = 0;
};

}  // namespace fungal
