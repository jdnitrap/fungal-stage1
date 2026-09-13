#pragma once
#include "../../include/types.hpp"
#include <string>

namespace fungal {

class StructuralAnalyzer {
public:
    static CodeStats analyze(const std::string& code);

private:
    static int count_nesting_depth(const std::string& code);
    static int count_branches(const std::string& code);
    static int count_functions(const std::string& code);
    static int count_tokens(const std::string& code);
};

}  // namespace fungal
