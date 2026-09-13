#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace fungal {

struct NeuralLayer {
    std::string name;
    int input_size;
    int output_size;
    std::vector<double> weights;
    std::vector<double> biases;
};

class NeuralEmbedding {
public:
    NeuralEmbedding(int input_dim, int hidden_dim, int output_dim);

    void initialize_weights();
    std::vector<double> forward_pass(const std::vector<double>& input);
    void train_on_example(const std::vector<double>& input, const std::vector<double>& expected_output);

    double get_loss() const;
    void set_learning_rate(double rate);

    std::vector<double> extract_features(const std::string& input_text);
    void update_weights(const std::vector<double>& gradients);

    std::string describe_network() const;

private:
    int input_dim_;
    int hidden_dim_;
    int output_dim_;
    double learning_rate_;
    double current_loss_;

    NeuralLayer hidden_layer_;
    NeuralLayer output_layer_;

    std::vector<double> sigmoid(const std::vector<double>& input);
    std::vector<double> relu(const std::vector<double>& input);
    double sigmoid_derivative(double x);
};

class HybridReasoner {
public:
    HybridReasoner(std::shared_ptr<NeuralEmbedding> neural, const std::string& node_id);

    std::string symbolic_reasoning(const std::map<std::string, double>& facts);
    std::vector<double> neural_reasoning(const std::vector<double>& input);

    std::string make_hybrid_decision(const std::map<std::string, double>& facts,
                                     const std::vector<double>& neural_input);

    void train_on_outcome(const std::vector<double>& input, bool successful);
    double get_hybrid_confidence() const;

    std::string describe_hybrid_reasoning() const;

private:
    std::shared_ptr<NeuralEmbedding> neural_model_;
    std::string node_id_;
    int successful_neural_predictions_;
    int total_neural_predictions_;
    int successful_symbolic_predictions_;
    int total_symbolic_predictions_;

    double calculate_weighting() const;
};

class ContinuousLearning {
public:
    ContinuousLearning();

    void process_new_data(const std::vector<double>& data);
    void adapt_to_distribution_shift(const std::string& shift_type);

    void online_learn(const std::vector<double>& input, const std::vector<double>& output);
    std::string get_learning_status() const;

    std::map<std::string, double> get_learned_representations() const;
    void update_model(const std::string& model_version);

private:
    int data_points_processed_;
    std::map<std::string, int> distribution_shifts_;
    int adaptations_made_;
};

}  // namespace fungal
