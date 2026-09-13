#include "substrate.hpp"
#include <algorithm>
#include <chrono>

namespace fungal {

Substrate::Substrate() {}

Substrate::~Substrate() {}

void Substrate::broadcast_signal(const Signal& signal) {
    std::lock_guard<std::mutex> lock(substrate_mutex_);

    for (auto& node_id : active_nodes_) {
        signal_queues_[node_id].push(signal);
    }
}

std::vector<Signal> Substrate::read_signals(const std::string& target_id, int limit) {
    std::lock_guard<std::mutex> lock(substrate_mutex_);

    std::vector<Signal> signals;
    auto& queue = signal_queues_[target_id];

    int count = 0;
    while (!queue.empty() && count < limit) {
        signals.push_back(queue.front());
        queue.pop();
        count++;
    }

    return signals;
}

void Substrate::set_condition(const std::string& name, double value) {
    std::lock_guard<std::mutex> lock(substrate_mutex_);

    conditions_[name] = {
        name,
        value,
        static_cast<double>(std::chrono::system_clock::now().time_since_epoch().count())
    };
}

EnvironmentalCondition Substrate::get_condition(const std::string& name) {
    std::lock_guard<std::mutex> lock(substrate_mutex_);

    auto it = conditions_.find(name);
    if (it != conditions_.end()) {
        return it->second;
    }
    return {name, 0.0, 0.0};
}

void Substrate::register_node(const std::string& node_id) {
    std::lock_guard<std::mutex> lock(substrate_mutex_);

    auto it = std::find(active_nodes_.begin(), active_nodes_.end(), node_id);
    if (it == active_nodes_.end()) {
        active_nodes_.push_back(node_id);
        signal_queues_[node_id] = std::queue<Signal>();
    }
}

void Substrate::unregister_node(const std::string& node_id) {
    std::lock_guard<std::mutex> lock(substrate_mutex_);

    auto it = std::find(active_nodes_.begin(), active_nodes_.end(), node_id);
    if (it != active_nodes_.end()) {
        active_nodes_.erase(it);
        signal_queues_.erase(node_id);
    }
}

std::vector<std::string> Substrate::get_active_nodes() const {
    std::lock_guard<std::mutex> lock(substrate_mutex_);
    return active_nodes_;
}

size_t Substrate::signal_queue_size() const {
    std::lock_guard<std::mutex> lock(substrate_mutex_);

    size_t total = 0;
    for (const auto& pair : signal_queues_) {
        total += pair.second.size();
    }
    return total;
}

void Substrate::clear_old_signals(double age_threshold) {
    std::lock_guard<std::mutex> lock(substrate_mutex_);

    double current_time = static_cast<double>(std::chrono::system_clock::now().time_since_epoch().count());

    for (auto& pair : signal_queues_) {
        std::queue<Signal> filtered;
        while (!pair.second.empty()) {
            Signal sig = pair.second.front();
            pair.second.pop();
            if (current_time - sig.timestamp < age_threshold) {
                filtered.push(sig);
            }
        }
        pair.second = filtered;
    }
}

}  // namespace fungal
