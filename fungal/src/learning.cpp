#include "learning.hpp"
#include <sstream>
#include <algorithm>
#include <cmath>

namespace fungal {

AdaptiveLearning::AdaptiveLearning() {}

void AdaptiveLearning::record_source_outcome(const std::string& source_id, bool correct) {
    if (source_trusts_.find(source_id) == source_trusts_.end()) {
        source_trusts_[source_id] = {source_id, 0.5, 0, 0};
    }

    auto& trust = source_trusts_[source_id];
    trust.total_predictions++;

    if (correct) {
        trust.correct_predictions++;
    }

    trust.trust_score = static_cast<double>(trust.correct_predictions) / trust.total_predictions;
    normalize_trust_scores();
}

void AdaptiveLearning::update_trust_score(const std::string& source_id, double adjustment) {
    if (source_trusts_.find(source_id) != source_trusts_.end()) {
        source_trusts_[source_id].trust_score += adjustment;
        source_trusts_[source_id].trust_score = std::min(1.0, std::max(0.0, source_trusts_[source_id].trust_score));
    }
}

double AdaptiveLearning::get_trust_score(const std::string& source_id) const {
    auto it = source_trusts_.find(source_id);
    if (it != source_trusts_.end()) {
        return it->second.trust_score;
    }
    return 0.5;
}

std::vector<SourceTrust> AdaptiveLearning::get_trusted_sources(double min_trust) const {
    std::vector<SourceTrust> result;

    for (const auto& pair : source_trusts_) {
        if (pair.second.trust_score >= min_trust) {
            result.push_back(pair.second);
        }
    }

    std::sort(result.begin(), result.end(),
              [](const SourceTrust& a, const SourceTrust& b) {
                  return a.trust_score > b.trust_score;
              });

    return result;
}

void AdaptiveLearning::weight_by_source_trust(std::map<std::string, double>& values, const std::string& source) {
    double trust = get_trust_score(source);

    for (auto& pair : values) {
        pair.second *= trust;
    }
}

std::vector<std::string> AdaptiveLearning::get_most_trusted_sources(int count) const {
    auto trusted = get_trusted_sources(0.0);

    std::vector<std::string> result;
    for (int i = 0; i < std::min(count, (int)trusted.size()); ++i) {
        result.push_back(trusted[i].source_id);
    }

    return result;
}

std::string AdaptiveLearning::describe_learned_trusts() const {
    std::stringstream ss;
    ss << "Learned Source Trusts:\n";

    auto trusted = get_trusted_sources(0.0);
    for (const auto& source : trusted) {
        ss << "  " << source.source_id << ": " << (source.trust_score * 100.0) << "% ("
           << source.correct_predictions << "/" << source.total_predictions << ")\n";
    }

    return ss.str();
}

void AdaptiveLearning::learn_domain_pattern(const std::string& domain, const std::string& pattern) {
    domain_patterns_[domain].push_back(pattern);
}

bool AdaptiveLearning::recognize_domain_pattern(const std::string& domain, const std::string& pattern) {
    auto it = domain_patterns_.find(domain);
    if (it == domain_patterns_.end()) return false;

    for (const auto& p : it->second) {
        if (p == pattern) return true;
    }

    return false;
}

void AdaptiveLearning::normalize_trust_scores() {
    double total = 0.0;
    for (auto& pair : source_trusts_) {
        total += pair.second.trust_score;
    }

    if (total > 0) {
        for (auto& pair : source_trusts_) {
            pair.second.trust_score /= source_trusts_.size();
        }
    }
}

ReinforcementLoop::ReinforcementLoop() {}

void ReinforcementLoop::observe_reward(const std::string& strategy, double reward) {
    strategy_attempts_[strategy]++;
    strategy_total_rewards_[strategy] += reward;
}

void ReinforcementLoop::observe_penalty(const std::string& strategy, double penalty) {
    strategy_attempts_[strategy]++;
    strategy_total_rewards_[strategy] -= penalty;
}

double ReinforcementLoop::get_strategy_score(const std::string& strategy) const {
    auto it = strategy_values_.find(strategy);
    if (it != strategy_values_.end()) {
        return it->second;
    }
    return 0.0;
}

std::string ReinforcementLoop::get_best_strategy() const {
    if (strategy_values_.empty()) return "default";

    std::string best = strategy_values_.begin()->first;
    double best_score = strategy_values_.begin()->second;

    for (const auto& pair : strategy_values_) {
        if (pair.second > best_score) {
            best_score = pair.second;
            best = pair.first;
        }
    }

    return best;
}

void ReinforcementLoop::update_strategy_value(const std::string& strategy, double value_change) {
    strategy_values_[strategy] += value_change;
}

std::vector<std::pair<std::string, double>> ReinforcementLoop::rank_strategies() const {
    std::vector<std::pair<std::string, double>> result(strategy_values_.begin(), strategy_values_.end());

    std::sort(result.begin(), result.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });

    return result;
}

void ReinforcementLoop::record_episode(const std::string& strategy, bool successful, double outcome_value) {
    if (successful) {
        observe_reward(strategy, outcome_value);
    } else {
        observe_penalty(strategy, outcome_value);
    }

    update_value_estimates();
}

double ReinforcementLoop::get_average_reward() const {
    if (strategy_total_rewards_.empty()) return 0.0;

    double total = 0.0;
    for (const auto& pair : strategy_total_rewards_) {
        total += pair.second;
    }

    return total / strategy_total_rewards_.size();
}

std::string ReinforcementLoop::describe_learned_strategies() const {
    std::stringstream ss;
    ss << "Learned Strategy Values:\n";

    auto ranked = rank_strategies();
    for (const auto& pair : ranked) {
        ss << "  " << pair.first << ": " << pair.second << "\n";
    }

    ss << "Average Reward: " << get_average_reward() << "\n";

    return ss.str();
}

void ReinforcementLoop::update_value_estimates() {
    for (auto& pair : strategy_values_) {
        auto attempts_it = strategy_attempts_.find(pair.first);
        auto rewards_it = strategy_total_rewards_.find(pair.first);

        if (attempts_it != strategy_attempts_.end() && rewards_it != strategy_total_rewards_.end()) {
            pair.second = rewards_it->second / std::max(1, attempts_it->second);
        }
    }
}

}  // namespace fungal
