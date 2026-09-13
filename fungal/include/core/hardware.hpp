#pragma once

#include <cstdint>
#include <string>

namespace fungal::core {

struct HardwareInfo {
    uint32_t cpu_cores;
    uint64_t total_memory_bytes;
    std::string os_name;
    std::string arch;
};

class HardwareDetector {
public:
    static HardwareInfo detect();
};

}  // namespace fungal::core
