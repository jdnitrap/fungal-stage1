#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>

namespace fungal {

struct Signal {
    std::string type;
    std::string source;
    std::string content;
    double timestamp;
    double priority;
};

struct EnvironmentalCondition {
    std::string name;
    double value;
    double timestamp;
};

class Substrate {
public:
    Substrate();
    ~Substrate();

    void broadcast_signal(const Signal& signal);
    std::vector<Signal> read_signals(const std::string& target_id, int limit = 100);

    void set_condition(const std::string& name, double value);
    EnvironmentalCondition get_condition(const std::string& name);

    void register_node(const std::string& node_id);
    void unregister_node(const std::string& node_id);
    std::vector<std::string> get_active_nodes() const;

    size_t signal_queue_size() const;
    void clear_old_signals(double age_threshold);

private:
    std::map<std::string, std::queue<Signal>> signal_queues_;
    std::map<std::string, EnvironmentalCondition> conditions_;
    std::vector<std::string> active_nodes_;
    mutable std::mutex substrate_mutex_;
};

}  // namespace fungal
