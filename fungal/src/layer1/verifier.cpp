#include "verifier.hpp"
#include <algorithm>

namespace fungal {

VerificationStatus RelationshipVerifier::verify_recursion_nesting_claim(
    const std::vector<double>& recursive_samples,
    const std::vector<double>& non_recursive_samples,
    double threshold) const {

    if (recursive_samples.empty() || non_recursive_samples.empty()) {
        return VerificationStatus::UNVERIFIED;
    }

    double recursive_avg = 0.0;
    for (double sample : recursive_samples) {
        recursive_avg += sample;
    }
    recursive_avg /= recursive_samples.size();

    double non_recursive_avg = 0.0;
    for (double sample : non_recursive_samples) {
        non_recursive_avg += sample;
    }
    non_recursive_avg /= non_recursive_samples.size();

    double difference = recursive_avg - non_recursive_avg;

    if (difference > threshold) {
        return VerificationStatus::CONFIRMED;
    } else if (difference < -threshold) {
        return VerificationStatus::CONTRADICTED;
    }

    return VerificationStatus::UNVERIFIED;
}

std::vector<VerifiedRelationship> RelationshipVerifier::verify_concept_relationships(
    const std::vector<std::string>& concepts) const {

    std::vector<VerifiedRelationship> relationships;

    if (concepts.size() >= 2) {
        for (size_t i = 0; i < concepts.size() - 1; ++i) {
            VerifiedRelationship rel;
            rel.concept1 = concepts[i];
            rel.concept2 = concepts[i + 1];
            rel.relationship_type = "prerequisite";
            rel.confidence_score = 0.8;
            rel.status = VerificationStatus::CONFIRMED;
            relationships.push_back(rel);
        }
    }

    return relationships;
}

}  // namespace fungal
