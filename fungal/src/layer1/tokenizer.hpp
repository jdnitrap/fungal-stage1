#pragma once

#include "../../include/types.hpp"
#include <string>
#include <vector>

namespace fungal {

struct Token {
    std::string value;
    std::string type;
    int line = 0;
    int column = 0;
};

class Tokenizer {
public:
    std::vector<Token> tokenize(const std::string& code);

private:
    bool is_keyword(const std::string& word) const;
    bool is_operator(char c) const;
    bool is_whitespace(char c) const;
};

}  // namespace fungal
