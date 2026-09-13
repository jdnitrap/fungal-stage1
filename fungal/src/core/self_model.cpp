#include "core/self_model.hpp"
#include <cmath>
#include <algorithm>

namespace fungal::core {

SelfModel::SelfModel() {
    // Initialize with one task type (v1: single type only)
    models_.resize(1, TaskTypeStats{});
}

double SelfModel::predict_success(int task_type_id) {
    auto& stats = get_or_create(task_type_id);
    // Return μ as the prediction
    return stats.success_mean;
}

double SelfModel::get_uncertainty(int task_type_id) const {
    if (task_type_id >= static_cast<int>(models_.size())) {
        return 0.3;  // default if not found
    }
    return models_[task_type_id].success_stddev;
}

void SelfModel::update_from_outcome(int task_type_id, bool outcome_correct, double predicted_prob) {
    auto& stats = get_or_create(task_type_id);

    // Update belief: Bayesian adjustment of μ and σ
    update_belief(stats, outcome_correct);

    // ACCURACY: did our strategy's claim match ground truth?
    // Separate signal: binary outcome of "was the claim right?"
    stats.total_predictions++;
    if (outcome_correct) {
        stats.accurate_predictions++;
    }

    // CALIBRATION: is our predicted probability justified by empirical frequency?
    // Track empirical success rate separately from accuracy
    double alpha = 0.15;  // smoothing factor for frequency tracking
    stats.empirical_success_rate = alpha * (outcome_correct ? 1.0 : 0.0) +
                                   (1.0 - alpha) * stats.empirical_success_rate;

    // Calibration error: how far is predicted probability from empirical frequency?
    // This measures: "when we predict P(success), do we actually see that frequency?"
    // Example: predict 70% but empirical is 40% → error = 0.3
    stats.calibration_error = std::abs(predicted_prob - stats.empirical_success_rate);
}

double SelfModel::get_calibration_error(int task_type_id) const {
    if (task_type_id >= static_cast<int>(models_.size())) {
        return 0.0;
    }
    return models_[task_type_id].calibration_error;
}

double SelfModel::get_accuracy(int task_type_id) const {
    if (task_type_id >= static_cast<int>(models_.size())) {
        return 0.5;
    }
    const auto& stats = models_[task_type_id];
    if (stats.total_predictions == 0) {
        return 0.5;
    }
    return static_cast<double>(stats.accurate_predictions) / stats.total_predictions;
}

double SelfModel::get_empirical_success_rate(int task_type_id) const {
    if (task_type_id >= static_cast<int>(models_.size())) {
        return 0.5;
    }
    return models_[task_type_id].empirical_success_rate;
}

TaskTypeStats& SelfModel::get_or_create(int task_type_id) {
    if (task_type_id < 0) {
        task_type_id = 0;
    }
    if (task_type_id >= static_cast<int>(models_.size())) {
        models_.resize(task_type_id + 1, TaskTypeStats{});
    }
    return models_[task_type_id];
}

TaskTypeStats SelfModel::get_stats(int task_type_id) const {
    if (task_type_id < 0 || task_type_id >= static_cast<int>(models_.size())) {
        return TaskTypeStats{};
    }
    return models_[task_type_id];
}

std::vector<TaskTypeStats> SelfModel::export_all() const {
    return models_;
}

void SelfModel::import_all(const std::vector<TaskTypeStats>& stats) {
    if (stats.empty()) {
        models_.assign(1, TaskTypeStats{});
        return;
    }
    models_ = stats;
}

void SelfModel::update_belief(TaskTypeStats& stats, bool outcome_correct) {
    // Bayesian update: outcome informs μ and σ
    // μ: mean success probability
    // σ: uncertainty in that estimate

    double outcome_value = outcome_correct ? 1.0 : 0.0;

    // Update mean: move toward observed outcome with adaptive learning rate
    // Higher uncertainty → faster learning (more plasticity when uncertain)
    double learning_rate = 0.15 * (1.0 + stats.success_stddev);
    stats.success_mean = (1.0 - learning_rate) * stats.success_mean +
                         learning_rate * outcome_value;

    // Update uncertainty based on prediction error
    // If prediction was close to outcome, reduce uncertainty (confidence increases)
    // If prediction was far, increase uncertainty (we need more info)
    double prediction_error = std::abs(outcome_value - stats.success_mean);

    // Scale σ adjustment by prediction error (not fixed multipliers)
    // Good prediction (error < 0.1): reduce σ by ~2%
    // Medium error (0.1-0.3): reduce σ by ~1%
    // Large error (>0.3): increase σ by error amount
    double sigma_adjustment;
    if (prediction_error < 0.1) {
        sigma_adjustment = 0.98;  // tighten confidence
    } else if (prediction_error < 0.3) {
        sigma_adjustment = 0.99;  // slight tightening
    } else {
        sigma_adjustment = 1.0 + (prediction_error * 0.2);  // scale up by error
    }
    stats.success_stddev *= sigma_adjustment;

    // Clamp σ to reasonable range [0.05, 0.5]
    stats.success_stddev = std::max(0.05, std::min(0.5, stats.success_stddev));
}

}  // namespace fungal::core
