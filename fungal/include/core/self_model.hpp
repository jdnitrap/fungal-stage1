#pragma once

#include <vector>
#include <cmath>
#include <algorithm>

namespace fungal::core {

// Predictive self-model: tracks capability + uncertainty + calibration
// Separate signals to avoid statistical confusion:
// - accuracy: did prediction match ground truth?
// - calibration: were predicted probabilities consistent with empirical frequency?

struct TaskTypeStats {
    // Bayesian estimates for a single task type
    double success_mean = 0.5;      // μ: predicted success probability
    double success_stddev = 0.3;    // σ: uncertainty in prediction

    // Calibration tracking: is the system's confidence justified?
    // Track predicted vs empirical frequency to separate confidence quality
    // from success rate itself
    int total_predictions = 0;
    int accurate_predictions = 0;   // predictions matched outcome
    double empirical_success_rate = 0.5;  // rolling average of actual success
    double calibration_error = 0.0; // divergence between predicted and empirical

    // Capability scores (used for strategy selection)
    double analyze_capability = 0.5;
    double verify_capability = 0.5;
};

class SelfModel {
public:
    SelfModel();

    // Predict success probability for a task (with uncertainty)
    // Returns predicted P(success) for this task type
    double predict_success(int task_type_id);

    // Get current uncertainty (σ) for a task type
    double get_uncertainty(int task_type_id) const;

    // Update after observing ground truth
    // outcome_correct: true if strategy's claim matched oracle (accuracy signal)
    // predicted_prob: what probability did we assign? (for calibration tracking)
    void update_from_outcome(int task_type_id, bool outcome_correct, double predicted_prob);

    // Get calibration error (are predicted probabilities justified?)
    double get_calibration_error(int task_type_id) const;

    // Get accuracy (% of predictions that matched outcomes)
    double get_accuracy(int task_type_id) const;

    // Get empirical success rate from recent observations
    double get_empirical_success_rate(int task_type_id) const;

    // Get or create stats for task type
    TaskTypeStats& get_or_create(int task_type_id);
    TaskTypeStats get_stats(int task_type_id) const;

    // Stage1 checkpoint support
    std::vector<TaskTypeStats> export_all() const;
    void import_all(const std::vector<TaskTypeStats>& stats);

private:
    std::vector<TaskTypeStats> models_;
    static constexpr size_t CALIBRATION_WINDOW = 50;  // rolling window for calibration

    // Bayesian update: strengthen belief if outcome matches prediction
    void update_belief(TaskTypeStats& stats, bool ground_truth);
};

}  // namespace fungal::core
