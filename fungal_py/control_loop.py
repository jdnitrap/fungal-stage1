"""Main control loop: predict → act → observe → learn."""

from dataclasses import dataclass
from typing import List, Optional

from hardware import HardwareAwareScheduler
from self_model import SelfModel
from energy_budget import EnergyBudget
from strategy import Strategy
from oracle import Oracle


@dataclass
class CycleResult:
    """Result of one control loop cycle."""
    code_snippet: str
    predicted_success: float
    strategy_claim: bool
    energy_spent: int
    oracle_ground_truth: bool
    prediction_correct: bool
    system_had_energy: bool


class ControlLoop:
    """Closed feedback loop: predict → energy gate → strategy → oracle → learn."""

    BASE_CYCLE_COST = 10  # base units per cycle

    def __init__(self, oracle: Oracle, strategy: Strategy):
        self.oracle = oracle
        self.strategy = strategy
        self.hardware_scheduler = HardwareAwareScheduler()
        self.self_model = SelfModel()
        self.energy_budget = EnergyBudget()
        self.total_cycles_run = 0
        self.cycles_that_ran = 0

    def initialize_from_hardware(self):
        """Detect hardware and scale energy/behavior."""
        profile = self.hardware_scheduler.get_profile()
        self.energy_budget.set_budget_from_hardware(profile)
        self.energy_cost_scale = profile['energy_cost_scale']

    def run_cycle(self, code_snippet: str, task_id: int = 0) -> CycleResult:
        """Execute one cycle of the control loop."""
        self.total_cycles_run += 1

        # 1. SENSE & PREDICT: What does self-model predict?
        predicted_success = self.self_model.predict_success(task_id)

        # 2. GATE: Can we afford this cycle?
        energy_cost = int(self.BASE_CYCLE_COST * self.energy_cost_scale)
        has_energy = self.energy_budget.spend_for_cycle(energy_cost)

        result = CycleResult(
            code_snippet=code_snippet,
            predicted_success=predicted_success,
            strategy_claim=False,
            energy_spent=energy_cost,
            oracle_ground_truth=False,
            prediction_correct=False,
            system_had_energy=has_energy
        )

        if not has_energy:
            return result  # Cycle halts; no learning

        self.cycles_that_ran += 1

        # 3. GENERATE: What does strategy claim?
        strategy_claim = self.strategy.apply(code_snippet)
        result.strategy_claim = strategy_claim

        # 4. EVALUATE: What's the oracle ground truth?
        oracle_ground_truth = self.oracle.has_bug(code_snippet)
        result.oracle_ground_truth = oracle_ground_truth

        # 5. COMPARE: Is strategy correct?
        prediction_correct = (strategy_claim == oracle_ground_truth)
        result.prediction_correct = prediction_correct

        # 6. COMMIT & ACT: Update energy and learning
        self.energy_budget.refund_outcome(prediction_correct, energy_cost)
        self.self_model.update_from_outcome(task_id, prediction_correct, predicted_success)

        return result

    def run_cycles(self, code_snippets: List[str], task_id: int = 0) -> List[CycleResult]:
        """Run multiple cycles."""
        results = []
        for snippet in code_snippets:
            result = self.run_cycle(snippet, task_id)
            results.append(result)
        return results
