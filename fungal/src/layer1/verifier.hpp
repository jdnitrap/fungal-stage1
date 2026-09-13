#pragma once
#include "../../include/types.hpp"
#include <vector>

namespace fungal {

class RelationshipVerifier {
public:
    VerificationStatus verify_recursion_nesting_claim(
        const std::vector<double>& recursive_samples,
        const std::vector<double>& non_recursive_samples,
        double threshold) const;

    std::vector<VerifiedRelationship> verify_concept_relationships(
        const std::vector<std::string>& concepts) const;
};

}  // namespace fungal
