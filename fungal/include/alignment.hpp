#pragma once

#include <string>
#include <map>
#include <vector>

namespace fungal {

struct ValueScore {
    std::string value;
    double alignment_score;
};

class ValueAlignment {
public:
    ValueAlignment();

    void set_core_value(const std::string& value, double weight);
    void evaluate_action(const std::string& action, const std::map<std::string, double>& impacts);

    bool is_aligned(const std::string& action) const;
    double get_alignment_score(const std::string& action) const;

    void record_outcome(const std::string& action, bool caused_harm);
    bool detect_misalignment(const std::string& action) const;

    std::vector<ValueScore> get_core_values() const;
    std::string describe_alignment() const;

    void learn_from_feedback(const std::string& action, bool successful, bool harmful);

private:
    std::map<std::string, double> core_values_;
    std::map<std::string, std::vector<double>> action_impacts_;
    std::map<std::string, int> harmful_actions_;
    std::map<std::string, int> successful_actions_;

    double calculate_harm_likelihood(const std::string& action) const;
};

}  // namespace fungal
