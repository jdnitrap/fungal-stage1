#include "hardware.hpp"
#include <thread>
#include <unistd.h>

#ifdef _WIN32
    #include <windows.h>
#elif __APPLE__
    #include <sys/sysctl.h>
#else
    #include <sys/sysinfo.h>
#endif

namespace fungal {

HardwareInfo HardwareDetector::cached_info_{0, 0, false, "", false, 0, "", ""};
bool HardwareDetector::initialized_ = false;

HardwareInfo HardwareDetector::detect() {
    if (initialized_) return cached_info_;

    cached_info_.cpu_cores = get_available_cores();
    cached_info_.total_memory_bytes = get_available_memory();
    cached_info_.has_gpu = has_gpu_support();
    cached_info_.gpu_type = "unknown";
    cached_info_.is_distributed = false;
    cached_info_.node_id = 0;
    cached_info_.os_name = get_os();
    cached_info_.arch = get_architecture();

    initialized_ = true;
    return cached_info_;
}

uint32_t HardwareDetector::get_available_cores() {
    return std::thread::hardware_concurrency();
}

uint64_t HardwareDetector::get_available_memory() {
#ifdef _WIN32
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return status.ullTotalPhys;
#elif __APPLE__
    int64_t mem;
    size_t len = sizeof(mem);
    sysctlbyname("hw.memsize", &mem, &len, NULL, 0);
    return mem;
#else
    struct sysinfo info;
    sysinfo(&info);
    return (uint64_t)info.totalram * info.mem_unit;
#endif
}

bool HardwareDetector::has_gpu_support() {
    return false;
}

std::string HardwareDetector::get_os() {
#ifdef _WIN32
    return "Windows";
#elif __APPLE__
    return "macOS";
#elif __linux__
    return "Linux";
#else
    return "Unknown";
#endif
}

std::string HardwareDetector::get_architecture() {
#ifdef __x86_64__
    return "x86_64";
#elif __arm__
    return "ARM";
#elif __aarch64__
    return "ARM64";
#else
    return "Unknown";
#endif
}

}  // namespace fungal
