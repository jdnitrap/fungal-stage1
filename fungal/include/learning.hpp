#pragma once

#include <string>
#include <map>
#include <vector>

namespace fungal {

struct SourceTrust {
    std::string source_id;
    double trust_score;
    int correct_predictions;
    int total_predictions;
};

class AdaptiveLearning {
public:
    AdaptiveLearning();

    void record_source_outcome(const std::string& source_id, bool correct);
    void update_trust_score(const std::string& source_id, double adjustment);

    double get_trust_score(const std::string& source_id) const;
    std::vector<SourceTrust> get_trusted_sources(double min_trust = 0.6) const;

    void weight_by_source_trust(std::map<std::string, double>& values, const std::string& source);

    std::vector<std::string> get_most_trusted_sources(int count = 5) const;
    std::string describe_learned_trusts() const;

    void learn_domain_pattern(const std::string& domain, const std::string& pattern);
    bool recognize_domain_pattern(const std::string& domain, const std::string& pattern);

private:
    std::map<std::string, SourceTrust> source_trusts_;
    std::map<std::string, std::vector<std::string>> domain_patterns_;

    void normalize_trust_scores();
};

class ReinforcementLoop {
public:
    ReinforcementLoop();

    void observe_reward(const std::string& strategy, double reward);
    void observe_penalty(const std::string& strategy, double penalty);

    double get_strategy_score(const std::string& strategy) const;
    std::string get_best_strategy() const;

    void update_strategy_value(const std::string& strategy, double value_change);
    std::vector<std::pair<std::string, double>> rank_strategies() const;

    void record_episode(const std::string& strategy, bool successful, double outcome_value);
    double get_average_reward() const;

    std::string describe_learned_strategies() const;

private:
    std::map<std::string, double> strategy_values_;
    std::map<std::string, int> strategy_attempts_;
    std::map<std::string, double> strategy_total_rewards_;

    void update_value_estimates();
};

}  // namespace fungal
