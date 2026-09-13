#include "core/hardware.hpp"
#include <thread>

#ifdef _WIN32
    #include <windows.h>
#elif __APPLE__
    #include <sys/sysctl.h>
    #include <sys/types.h>
#else
    #include <unistd.h>
#endif

namespace fungal::core {

HardwareInfo HardwareDetector::detect() {
    HardwareInfo info;

    // Detect CPU cores
#ifdef _WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    info.cpu_cores = sysinfo.dwNumberOfProcessors;
    info.os_name = "Windows";
#else
    info.cpu_cores = std::thread::hardware_concurrency();
    #ifdef __APPLE__
        info.os_name = "macOS";
    #else
        info.os_name = "Linux";
    #endif
#endif

    // Detect total memory
#ifdef _WIN32
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    info.total_memory_bytes = statex.ullTotalPhys;
#elif __APPLE__
    int64_t mem;
    size_t size = sizeof(mem);
    sysctlbyname("hw.memsize", &mem, &size, nullptr, 0);
    info.total_memory_bytes = static_cast<uint64_t>(mem);
#else
    info.total_memory_bytes = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGE_SIZE);
#endif

    // Detect architecture
#ifdef __x86_64__
    info.arch = "x86_64";
#elif __i386__
    info.arch = "x86";
#elif __aarch64__
    info.arch = "ARM64";
#elif __arm__
    info.arch = "ARM";
#else
    info.arch = "unknown";
#endif

    // Ensure sensible defaults
    if (info.cpu_cores == 0) {
        info.cpu_cores = 4;
    }
    if (info.total_memory_bytes == 0) {
        info.total_memory_bytes = 8ULL * 1024 * 1024 * 1024;  // 8GB default
    }

    return info;
}

}  // namespace fungal::core
