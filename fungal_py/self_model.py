"""Bayesian self-model for success prediction and calibration."""

import math


class TaskTypeStats:
    """Statistics for a single task type."""
    def __init__(self):
        self.success_mean = 0.5  # μ: predicted success probability
        self.success_stddev = 0.2  # σ: uncertainty
        self.accurate_predictions = 0  # claim matched oracle?
        self.total_predictions = 0
        self.empirical_success_rate = 0.5
        self.calibration_error = 0.0


class SelfModel:
    """Bayesian predictor with separate accuracy and calibration tracking."""

    def __init__(self):
        self.task_stats = {}  # task_id -> TaskTypeStats

    def predict_success(self, task_id: int) -> float:
        """Predict probability of success for a task type."""
        if task_id not in self.task_stats:
            self.task_stats[task_id] = TaskTypeStats()
        return self.task_stats[task_id].success_mean

    def get_empirical_success_rate(self, task_id: int) -> float:
        """Return empirical success frequency over all cycles."""
        if task_id not in self.task_stats:
            return 0.5
        stats = self.task_stats[task_id]
        if stats.total_predictions == 0:
            return 0.5
        return stats.empirical_success_rate

    def get_calibration_error(self, task_id: int) -> float:
        """Return |predicted_prob - empirical_freq|."""
        if task_id not in self.task_stats:
            return 0.0
        return self.task_stats[task_id].calibration_error

    def update_from_outcome(self, task_id: int, outcome_correct: bool, predicted_prob: float):
        """Learn from observed outcome.

        Args:
            task_id: task type identifier
            outcome_correct: was the strategy claim correct? (binary)
            predicted_prob: what did self-model predict?
        """
        if task_id not in self.task_stats:
            self.task_stats[task_id] = TaskTypeStats()

        stats = self.task_stats[task_id]
        stats.total_predictions += 1

        # Accuracy: did claim match oracle?
        if outcome_correct:
            stats.accurate_predictions += 1

        # Empirical frequency tracking
        empirical_freq = stats.accurate_predictions / stats.total_predictions

        # Calibration: |predicted - empirical|
        stats.empirical_success_rate = empirical_freq
        stats.calibration_error = abs(predicted_prob - empirical_freq)

        # Update belief (μ/σ) with error-scaled Bayesian adjustment
        prediction_error = abs(predicted_prob - (1.0 if outcome_correct else 0.0))

        # Learning rate scales with uncertainty (σ)
        learning_rate = min(0.1, stats.success_stddev / 2.0)
        direction = 1.0 if outcome_correct else -1.0
        stats.success_mean += direction * learning_rate * (1.0 - stats.success_mean if direction > 0 else stats.success_mean)

        # Adjust σ: tight predictions that are correct → lower σ (98%)
        # Loose predictions or wrong predictions → higher σ
        if prediction_error < 0.1:
            stats.success_stddev *= 0.98
        elif prediction_error > 0.3:
            stats.success_stddev = min(0.5, stats.success_stddev * (1.0 + 0.2 * prediction_error))

        # Clamp to valid ranges
        stats.success_mean = max(0.0, min(1.0, stats.success_mean))
        stats.success_stddev = max(0.01, min(0.5, stats.success_stddev))
