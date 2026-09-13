#include "neural_integration.hpp"
#include <sstream>
#include <cmath>
#include <algorithm>
#include <random>

namespace fungal {

NeuralEmbedding::NeuralEmbedding(int input_dim, int hidden_dim, int output_dim)
    : input_dim_(input_dim), hidden_dim_(hidden_dim), output_dim_(output_dim),
      learning_rate_(0.01), current_loss_(0.0) {

    hidden_layer_ = {
        "hidden",
        input_dim,
        hidden_dim,
        std::vector<double>(input_dim * hidden_dim, 0.0),
        std::vector<double>(hidden_dim, 0.0)
    };

    output_layer_ = {
        "output",
        hidden_dim,
        output_dim,
        std::vector<double>(hidden_dim * output_dim, 0.0),
        std::vector<double>(output_dim, 0.0)
    };

    initialize_weights();
}

void NeuralEmbedding::initialize_weights() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-0.5, 0.5);

    for (auto& w : hidden_layer_.weights) {
        w = dis(gen);
    }
    for (auto& w : output_layer_.weights) {
        w = dis(gen);
    }
}

std::vector<double> NeuralEmbedding::forward_pass(const std::vector<double>& input) {
    std::vector<double> hidden_output(hidden_dim_, 0.0);

    for (int i = 0; i < hidden_dim_; ++i) {
        for (int j = 0; j < input_dim_; ++j) {
            hidden_output[i] += input[j] * hidden_layer_.weights[j * hidden_dim_ + i];
        }
        hidden_output[i] += hidden_layer_.biases[i];
    }

    hidden_output = relu(hidden_output);

    std::vector<double> output(output_dim_, 0.0);
    for (int i = 0; i < output_dim_; ++i) {
        for (int j = 0; j < hidden_dim_; ++j) {
            output[i] += hidden_output[j] * output_layer_.weights[j * output_dim_ + i];
        }
        output[i] += output_layer_.biases[i];
    }

    output = sigmoid(output);

    return output;
}

void NeuralEmbedding::train_on_example(const std::vector<double>& input, const std::vector<double>& expected_output) {
    auto predicted = forward_pass(input);

    current_loss_ = 0.0;
    for (size_t i = 0; i < expected_output.size(); ++i) {
        double error = expected_output[i] - predicted[i];
        current_loss_ += error * error;
    }
    current_loss_ /= expected_output.size();
}

double NeuralEmbedding::get_loss() const {
    return current_loss_;
}

void NeuralEmbedding::set_learning_rate(double rate) {
    learning_rate_ = rate;
}

std::vector<double> NeuralEmbedding::extract_features(const std::string& input_text) {
    std::vector<double> features(input_dim_, 0.0);

    for (size_t i = 0; i < std::min(input_text.length(), (size_t)input_dim_); ++i) {
        features[i] = (double)input_text[i] / 255.0;
    }

    return features;
}

void NeuralEmbedding::update_weights(const std::vector<double>& gradients) {
    if (gradients.size() != hidden_layer_.weights.size()) return;

    for (size_t i = 0; i < hidden_layer_.weights.size(); ++i) {
        hidden_layer_.weights[i] += learning_rate_ * gradients[i];
    }
}

std::string NeuralEmbedding::describe_network() const {
    std::stringstream ss;
    ss << "Neural Network:\n";
    ss << "  Input: " << input_dim_ << " dimensions\n";
    ss << "  Hidden: " << hidden_dim_ << " neurons\n";
    ss << "  Output: " << output_dim_ << " dimensions\n";
    ss << "  Learning Rate: " << learning_rate_ << "\n";
    ss << "  Current Loss: " << current_loss_ << "\n";

    return ss.str();
}

std::vector<double> NeuralEmbedding::sigmoid(const std::vector<double>& input) {
    std::vector<double> output;
    for (double x : input) {
        output.push_back(1.0 / (1.0 + std::exp(-x)));
    }
    return output;
}

std::vector<double> NeuralEmbedding::relu(const std::vector<double>& input) {
    std::vector<double> output;
    for (double x : input) {
        output.push_back(std::max(0.0, x));
    }
    return output;
}

double NeuralEmbedding::sigmoid_derivative(double x) {
    double sig = 1.0 / (1.0 + std::exp(-x));
    return sig * (1.0 - sig);
}

HybridReasoner::HybridReasoner(std::shared_ptr<NeuralEmbedding> neural, const std::string& node_id)
    : neural_model_(neural), node_id_(node_id), successful_neural_predictions_(0),
      total_neural_predictions_(0), successful_symbolic_predictions_(0),
      total_symbolic_predictions_(0) {}

std::string HybridReasoner::symbolic_reasoning(const std::map<std::string, double>& facts) {
    total_symbolic_predictions_++;

    double sum = 0.0;
    for (const auto& pair : facts) {
        sum += pair.second;
    }

    double average = facts.empty() ? 0.0 : sum / facts.size();

    if (average > 0.8) {
        successful_symbolic_predictions_++;
        return "strong_positive";
    } else if (average > 0.5) {
        successful_symbolic_predictions_++;
        return "positive";
    } else if (average > 0.2) {
        return "weak";
    }

    return "negative";
}

std::vector<double> HybridReasoner::neural_reasoning(const std::vector<double>& input) {
    total_neural_predictions_++;
    return neural_model_->forward_pass(input);
}

std::string HybridReasoner::make_hybrid_decision(const std::map<std::string, double>& facts,
                                                  const std::vector<double>& neural_input) {
    std::string symbolic_result = symbolic_reasoning(facts);
    std::vector<double> neural_result = neural_reasoning(neural_input);

    double neural_confidence = neural_result.empty() ? 0.0 : neural_result[0];
    double weighting = calculate_weighting();

    if (neural_confidence > 0.7 && weighting > 0.5) {
        successful_neural_predictions_++;
        return "neural_decision_" + symbolic_result;
    }

    return "hybrid_decision_" + symbolic_result;
}

void HybridReasoner::train_on_outcome(const std::vector<double>& input, bool successful) {
    std::vector<double> target(1, successful ? 1.0 : 0.0);
    neural_model_->train_on_example(input, target);
}

double HybridReasoner::get_hybrid_confidence() const {
    double symbolic_conf = total_symbolic_predictions_ > 0 ?
        static_cast<double>(successful_symbolic_predictions_) / total_symbolic_predictions_ : 0.0;
    double neural_conf = total_neural_predictions_ > 0 ?
        static_cast<double>(successful_neural_predictions_) / total_neural_predictions_ : 0.0;

    return (symbolic_conf + neural_conf) / 2.0;
}

std::string HybridReasoner::describe_hybrid_reasoning() const {
    std::stringstream ss;
    ss << "Hybrid Reasoner (" << node_id_ << "):\n";
    ss << "  Symbolic Success: " << successful_symbolic_predictions_ << "/" << total_symbolic_predictions_ << "\n";
    ss << "  Neural Success: " << successful_neural_predictions_ << "/" << total_neural_predictions_ << "\n";
    ss << "  Hybrid Confidence: " << (get_hybrid_confidence() * 100.0) << "%\n";

    return ss.str();
}

double HybridReasoner::calculate_weighting() const {
    if (total_neural_predictions_ == 0) return 0.5;
    if (total_symbolic_predictions_ == 0) return 0.5;

    double neural_rate = static_cast<double>(successful_neural_predictions_) / total_neural_predictions_;
    double symbolic_rate = static_cast<double>(successful_symbolic_predictions_) / total_symbolic_predictions_;

    return neural_rate > symbolic_rate ? 0.6 : 0.4;
}

ContinuousLearning::ContinuousLearning()
    : data_points_processed_(0), adaptations_made_(0) {}

void ContinuousLearning::process_new_data(const std::vector<double>& data) {
    data_points_processed_ += data.size();
}

void ContinuousLearning::adapt_to_distribution_shift(const std::string& shift_type) {
    distribution_shifts_[shift_type]++;
    adaptations_made_++;
}

void ContinuousLearning::online_learn(const std::vector<double>& input, const std::vector<double>& output) {
    data_points_processed_ += input.size();
}

std::string ContinuousLearning::get_learning_status() const {
    std::stringstream ss;
    ss << "Continuous Learning Status:\n";
    ss << "  Data Points Processed: " << data_points_processed_ << "\n";
    ss << "  Distribution Shifts Detected: " << distribution_shifts_.size() << "\n";
    ss << "  Adaptations Made: " << adaptations_made_ << "\n";

    return ss.str();
}

std::map<std::string, double> ContinuousLearning::get_learned_representations() const {
    std::map<std::string, double> representations;
    representations["data_processed_ratio"] = std::min(1.0, data_points_processed_ / 10000.0);
    representations["adaptation_rate"] = adaptations_made_ > 0 ? 0.8 : 0.2;

    return representations;
}

void ContinuousLearning::update_model(const std::string& model_version) {
}

}  // namespace fungal
