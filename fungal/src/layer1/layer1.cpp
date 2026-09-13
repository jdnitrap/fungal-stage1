#include "../../include/interfaces.hpp"
#include "tokenizer.hpp"
#include "analyzer.hpp"
#include "dictionary.hpp"
#include "verifier.hpp"
#include "builder.hpp"
#include <vector>

namespace fungal {

class Layer1SystemImpl : public Layer1System {
private:
    Tokenizer tokenizer;
    ConceptDictionary dictionary;
    RelationshipVerifier verifier;
    TemplateBuilder builder;
    std::vector<Claim> foundational_claims;

public:
    void initialize() override {
        dictionary.initialize_concepts();
        foundational_claims.clear();
    }

    void analyze_code(const std::string& code) override {
        CodeStats stats = StructuralAnalyzer::analyze(code);

        LearningTemplate tmpl = builder.build("recursion", dictionary);

        if (tmpl.topic_recognized && tmpl.valid) {
            Claim claim;
            claim.id = "recursion_pattern_detected";
            claim.statement = "Code exhibits recursive patterns with nesting";
            claim.confidence = 0.7;
            claim.verification_status = VerificationStatus::UNVERIFIED;
            claim.supporting_evidence.push_back(
                "max_nesting_depth: " + std::to_string(stats.max_nesting_depth));
            claim.supporting_evidence.push_back(
                "function_count: " + std::to_string(stats.function_count));
            foundational_claims.push_back(claim);
        }
    }

    std::vector<Claim> get_foundational_claims() const override {
        return foundational_claims;
    }
};

Layer1System* create_layer1_system() {
    return new Layer1SystemImpl();
}

}  // namespace fungal
