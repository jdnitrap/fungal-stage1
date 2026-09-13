#include "analyzer.hpp"
#include "tokenizer.hpp"

namespace fungal {

CodeStats StructuralAnalyzer::analyze(const std::string& code) {
    CodeStats stats;
    Tokenizer tokenizer;

    std::vector<Token> tokens = tokenizer.tokenize(code);
    stats.token_count = tokens.size();

    int current_depth = 0;
    int max_depth = 0;

    for (const auto& token : tokens) {
        if (token.value == "{") {
            current_depth++;
            if (current_depth > max_depth) {
                max_depth = current_depth;
            }
        } else if (token.value == "}") {
            current_depth--;
        } else if (token.value == "if" || token.value == "else" ||
                   token.value == "for" || token.value == "while") {
            stats.branch_count++;
        } else if (token.value == "def" || token.value == "class" || token.value == "void") {
            stats.function_count++;
        }
    }

    stats.max_nesting_depth = max_depth;
    return stats;
}

}  // namespace fungal
