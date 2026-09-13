#include "core/hardware_aware_scheduler.hpp"
#include "core/hardware.hpp"
#include <algorithm>

namespace fungal::core {

HardwareAwareScheduler::HardwareAwareScheduler() = default;

HardwareProfile HardwareAwareScheduler::detect_hardware() {
    // Use existing hardware detector
    auto hw_info = HardwareDetector::detect();

    current_profile_ = HardwareProfile{
        .cpu_cores = static_cast<int>(hw_info.cpu_cores),
        .memory_mb = hw_info.total_memory_bytes / (1024 * 1024),
        .os_name = hw_info.os_name,
        .architecture = hw_info.arch
    };

    detected_ = true;
    return current_profile_;
}

TaskBatchParameters HardwareAwareScheduler::compute_batch_parameters(const HardwareProfile& profile) {
    // Batch size from cores: use 1x cores up to a max of 16
    int batch_size = std::min(profile.cpu_cores, 16);
    if (batch_size < 1) batch_size = 1;

    // Energy cost scale from memory: scarce memory = higher costs (system more conservative)
    // Mapping: < 256MB = 1.5x cost, 256MB-1GB = 1.2x, 1-4GB = 1.0x, > 4GB = 0.8x
    double energy_scale;
    if (profile.memory_mb < 256) {
        energy_scale = 1.5;
    } else if (profile.memory_mb < 1024) {
        energy_scale = 1.2;
    } else if (profile.memory_mb < 4096) {
        energy_scale = 1.0;
    } else {
        energy_scale = 0.8;
    }

    // Max concurrent: capped by cores and available memory
    size_t max_concurrent = std::min(static_cast<size_t>(profile.cpu_cores),
                                     profile.memory_mb / 64);  // assume 64MB per task as baseline

    return TaskBatchParameters{
        .batch_size = batch_size,
        .energy_cost_scale = energy_scale,
        .max_concurrent_tasks = std::max(size_t(1), max_concurrent)
    };
}

HardwareProfile HardwareAwareScheduler::get_current_profile() const {
    return current_profile_;
}

}  // namespace fungal::core
