"""Hardware detection and profiling."""

import os
import platform


class HardwareInfo:
    """Detected hardware characteristics."""
    def __init__(self):
        self.cpu_cores = os.cpu_count() or 1
        # Estimate memory from /proc/meminfo on Linux, or default to 8GB
        self.memory_mb = self._detect_memory()
        self.os_name = platform.system()
        self.arch = platform.machine()

    def _detect_memory(self) -> int:
        """Detect system memory in MB."""
        try:
            if os.path.exists('/proc/meminfo'):
                with open('/proc/meminfo') as f:
                    for line in f:
                        if line.startswith('MemTotal:'):
                            kb = int(line.split()[1])
                            return kb // 1024
        except:
            pass
        return 8192  # Default to 8GB if detection fails


class HardwareAwareScheduler:
    """Scales behavior based on hardware."""

    def __init__(self):
        self.hardware = HardwareInfo()
        self.batch_size = min(self.hardware.cpu_cores, 16)
        # Energy cost scale: scarce hardware (< 4GB) = 1.5x, abundant (> 16GB) = 0.8x
        memory_scale = self.hardware.memory_mb / 8192.0
        if memory_scale < 0.5:
            self.energy_cost_scale = 1.5
        elif memory_scale > 2.0:
            self.energy_cost_scale = 0.8
        else:
            self.energy_cost_scale = 1.0 + (0.5 - memory_scale) * 0.5

    def get_profile(self):
        """Return current hardware profile."""
        return {
            'cpu_cores': self.hardware.cpu_cores,
            'memory_mb': self.hardware.memory_mb,
            'os': self.hardware.os_name,
            'arch': self.hardware.arch,
            'batch_size': self.batch_size,
            'energy_cost_scale': self.energy_cost_scale
        }
