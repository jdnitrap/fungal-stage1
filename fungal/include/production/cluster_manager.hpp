#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>

namespace fungal::production {

using json = nlohmann::json;

enum class NodeState {
    UNKNOWN,
    HEALTHY,
    DEGRADED,
    UNHEALTHY,
    DISCONNECTED
};

struct ClusterNode {
    std::string id;
    std::string hostname;
    int port;
    NodeState state;
    json metadata;
    std::string last_heartbeat;
};

struct ClusterConfig {
    std::string cluster_id;
    std::string node_id;
    std::vector<std::string> peer_addresses;
    int heartbeat_interval_ms;
    int election_timeout_ms;
};

class ClusterManager {
public:
    static ClusterManager& instance();

    // Initialize cluster
    void initialize(const ClusterConfig& config);

    // Add peer node
    void add_peer(const std::string& hostname, int port);
    void remove_peer(const std::string& node_id);

    // Get cluster information
    std::vector<ClusterNode> get_nodes() const;
    ClusterNode get_node(const std::string& node_id) const;
    size_t get_cluster_size() const;

    // Node state management
    void update_node_state(const std::string& node_id, NodeState state);
    NodeState get_node_state(const std::string& node_id) const;

    // Distributed consensus
    bool is_leader() const;
    std::string get_leader_id() const;
    void elect_new_leader();

    // Replication
    void replicate_state(const json& state);
    json get_replicated_state();

    // Cluster coordination
    bool send_message_to_node(const std::string& node_id, const json& message);
    void broadcast_message(const json& message);

    // Health monitoring
    void send_heartbeat();
    void process_heartbeat(const std::string& node_id);

    // Partition tolerance
    bool is_partitioned() const;
    std::vector<std::string> get_isolated_nodes() const;

    // Export cluster status
    json get_cluster_status() const;
    json export_cluster_info() const;

private:
    ClusterManager() = default;

    ClusterConfig config_;
    std::map<std::string, ClusterNode> nodes_;
    std::string leader_id_;
    bool initialized_ = false;

    void initialize_cluster_discovery();
    void start_heartbeat_monitor();
};

}  // namespace fungal::production
