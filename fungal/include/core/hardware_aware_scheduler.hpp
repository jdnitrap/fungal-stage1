#pragma once

#include <string>
#include <cstddef>

namespace fungal::core {

struct HardwareProfile {
    int cpu_cores;
    size_t memory_mb;
    std::string os_name;
    std::string architecture;
};

struct TaskBatchParameters {
    int batch_size;           // tasks per cycle
    double energy_cost_scale; // multiplier for operation costs based on resource scarcity
    size_t max_concurrent_tasks;
};

class HardwareAwareScheduler {
public:
    HardwareAwareScheduler();

    // Detect hardware at startup (or on explicit refresh)
    HardwareProfile detect_hardware();

    // Compute batch parameters from hardware profile
    TaskBatchParameters compute_batch_parameters(const HardwareProfile& profile);

    // Get current profile (for monitoring)
    HardwareProfile get_current_profile() const;

private:
    HardwareProfile current_profile_;
    bool detected_ = false;
};

}  // namespace fungal::core
