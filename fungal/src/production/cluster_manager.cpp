#include "production/cluster_manager.hpp"
#include <algorithm>

namespace fungal::production {

ClusterManager& ClusterManager::instance() {
    static ClusterManager instance;
    return instance;
}

void ClusterManager::initialize(const ClusterConfig& config) {
    config_ = config;

    ClusterNode self;
    self.id = config.node_id;
    self.hostname = "localhost";
    self.port = 0;
    self.state = NodeState::HEALTHY;

    nodes_[self.id] = self;

    for (const auto& peer_addr : config.peer_addresses) {
        // Parse hostname:port
        size_t colon_pos = peer_addr.find(':');
        if (colon_pos != std::string::npos) {
            std::string hostname = peer_addr.substr(0, colon_pos);
            int port = std::stoi(peer_addr.substr(colon_pos + 1));
            add_peer(hostname, port);
        }
    }

    leader_id_ = config.node_id;
    initialized_ = true;
}

void ClusterManager::add_peer(const std::string& hostname, int port) {
    ClusterNode peer;
    peer.hostname = hostname;
    peer.port = port;
    peer.state = NodeState::UNKNOWN;
    peer.id = hostname + ":" + std::to_string(port);

    nodes_[peer.id] = peer;
}

void ClusterManager::remove_peer(const std::string& node_id) {
    nodes_.erase(node_id);
}

std::vector<ClusterNode> ClusterManager::get_nodes() const {
    std::vector<ClusterNode> node_list;
    for (const auto& [id, node] : nodes_) {
        node_list.push_back(node);
    }
    return node_list;
}

ClusterNode ClusterManager::get_node(const std::string& node_id) const {
    auto it = nodes_.find(node_id);
    if (it != nodes_.end()) {
        return it->second;
    }

    ClusterNode empty;
    empty.state = NodeState::UNKNOWN;
    return empty;
}

size_t ClusterManager::get_cluster_size() const {
    return nodes_.size();
}

void ClusterManager::update_node_state(const std::string& node_id, NodeState state) {
    auto it = nodes_.find(node_id);
    if (it != nodes_.end()) {
        it->second.state = state;
        it->second.last_heartbeat = std::to_string(
            std::chrono::system_clock::now().time_since_epoch().count());
    }
}

NodeState ClusterManager::get_node_state(const std::string& node_id) const {
    auto node = get_node(node_id);
    return node.state;
}

bool ClusterManager::is_leader() const {
    return leader_id_ == config_.node_id;
}

std::string ClusterManager::get_leader_id() const {
    return leader_id_;
}

void ClusterManager::elect_new_leader() {
    // Simple election: select first healthy node
    for (auto& [id, node] : nodes_) {
        if (node.state == NodeState::HEALTHY) {
            leader_id_ = id;
            return;
        }
    }
    // Fallback to current node
    leader_id_ = config_.node_id;
}

void ClusterManager::replicate_state(const json& state) {
    // In production, send state to all peers
}

json ClusterManager::get_replicated_state() {
    // In production, collect state from majority of peers
    return json::object();
}

bool ClusterManager::send_message_to_node(const std::string& node_id, const json& message) {
    auto it = nodes_.find(node_id);
    if (it == nodes_.end()) {
        return false;
    }

    // In production, would send actual network message
    return it->second.state == NodeState::HEALTHY;
}

void ClusterManager::broadcast_message(const json& message) {
    for (auto& [id, node] : nodes_) {
        send_message_to_node(id, message);
    }
}

void ClusterManager::send_heartbeat() {
    auto self_it = nodes_.find(config_.node_id);
    if (self_it != nodes_.end()) {
        self_it->second.last_heartbeat = std::to_string(
            std::chrono::system_clock::now().time_since_epoch().count());
    }

    // In production, send heartbeat to all peers
}

void ClusterManager::process_heartbeat(const std::string& node_id) {
    update_node_state(node_id, NodeState::HEALTHY);
}

bool ClusterManager::is_partitioned() const {
    // Check if we can reach a majority of nodes
    int reachable = 0;
    int total = 0;

    for (const auto& [id, node] : nodes_) {
        if (node.state == NodeState::HEALTHY) {
            reachable++;
        }
        total++;
    }

    return reachable < (total / 2 + 1);
}

std::vector<std::string> ClusterManager::get_isolated_nodes() const {
    std::vector<std::string> isolated;

    for (const auto& [id, node] : nodes_) {
        if (node.state == NodeState::DISCONNECTED || node.state == NodeState::UNHEALTHY) {
            isolated.push_back(id);
        }
    }

    return isolated;
}

json ClusterManager::get_cluster_status() const {
    json status;
    status["cluster_id"] = config_.cluster_id;
    status["node_id"] = config_.node_id;
    status["is_leader"] = is_leader();
    status["leader_id"] = leader_id_;
    status["cluster_size"] = get_cluster_size();
    status["is_partitioned"] = is_partitioned();

    json nodes_array = json::array();
    for (const auto& node : get_nodes()) {
        json node_obj;
        node_obj["id"] = node.id;
        node_obj["hostname"] = node.hostname;
        node_obj["port"] = node.port;
        node_obj["state"] = static_cast<int>(node.state);
        nodes_array.push_back(node_obj);
    }
    status["nodes"] = nodes_array;

    return status;
}

json ClusterManager::export_cluster_info() const {
    return get_cluster_status();
}

void ClusterManager::initialize_cluster_discovery() {
    // In production, would implement peer discovery
}

void ClusterManager::start_heartbeat_monitor() {
    // In production, would start background heartbeat thread
}

}  // namespace fungal::production
