#pragma once

#include <string>
#include <cstdint>
#include <vector>

namespace fungal {

struct HardwareInfo {
    uint32_t cpu_cores;
    uint64_t total_memory_bytes;
    bool has_gpu;
    std::string gpu_type;
    bool is_distributed;
    uint32_t node_id;
    std::string os_name;
    std::string arch;
};

class HardwareDetector {
public:
    static HardwareInfo detect();
    static uint32_t get_available_cores();
    static uint64_t get_available_memory();
    static bool has_gpu_support();
    static std::string get_os();
    static std::string get_architecture();

private:
    static HardwareInfo cached_info_;
    static bool initialized_;
};

}  // namespace fungal
