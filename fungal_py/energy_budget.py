"""Energy budget management and resource constraints."""

import math


class EnergyBudget:
    """Hard resource constraint that gates cycle execution."""

    def __init__(self, initial_budget: int = 1000):
        self.current_budget = initial_budget
        self.total_refunded = 0
        self.total_spent = 0

    def set_budget_from_hardware(self, hardware_profile: dict):
        """Scale budget based on hardware capabilities."""
        cpu_cores = hardware_profile.get('cpu_cores', 4)
        memory_mb = hardware_profile.get('memory_mb', 8192)

        # Base budget scaled by: 1 + log(cores)/10 * memory_factor
        core_factor = 1.0 + math.log(cpu_cores) / 10.0 if cpu_cores > 0 else 1.0
        memory_factor = memory_mb / 8192.0

        self.current_budget = int(1000 * core_factor * (0.8 if memory_factor > 2.0 else memory_factor))

    def spend_for_cycle(self, cost: int) -> bool:
        """Attempt to spend energy for a cycle.

        Returns:
            True if budget available and spent; False if budget exhausted (cycle halts).
        """
        if self.current_budget < cost:
            return False
        self.current_budget -= cost
        self.total_spent += cost
        return True

    def refund_outcome(self, success: bool, cost_paid: int):
        """Refund based on outcome.

        Success gets partial refund (40%).
        Failure applies extra penalty (30 units).
        """
        if success:
            refund = int(cost_paid * 0.40)
            self.current_budget += refund
            self.total_refunded += refund
        else:
            penalty = 30
            self.current_budget -= penalty
            self.total_spent += penalty
