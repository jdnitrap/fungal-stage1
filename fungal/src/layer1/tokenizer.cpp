#include "tokenizer.hpp"
#include <cctype>

namespace fungal {

std::vector<Token> Tokenizer::tokenize(const std::string& code) {
    std::vector<Token> tokens;
    std::string word;
    int line = 1;
    int column = 0;

    for (char c : code) {
        if (c == '\n') {
            if (!word.empty()) {
                Token tok{word, is_keyword(word) ? "keyword" : "identifier", line, column};
                tokens.push_back(tok);
                word.clear();
            }
            line++;
            column = 0;
            continue;
        }

        if (is_whitespace(c)) {
            if (!word.empty()) {
                Token tok{word, is_keyword(word) ? "keyword" : "identifier", line, column};
                tokens.push_back(tok);
                word.clear();
            }
            column++;
            continue;
        }

        if (is_operator(c)) {
            if (!word.empty()) {
                Token tok{word, is_keyword(word) ? "keyword" : "identifier", line, column};
                tokens.push_back(tok);
                word.clear();
            }
            Token tok{std::string(1, c), "operator", line, column};
            tokens.push_back(tok);
            column++;
            continue;
        }

        word += c;
        column++;
    }

    if (!word.empty()) {
        Token tok{word, is_keyword(word) ? "keyword" : "identifier", line, column};
        tokens.push_back(tok);
    }

    return tokens;
}

bool Tokenizer::is_keyword(const std::string& word) const {
    static const std::vector<std::string> keywords = {
        "if", "else", "for", "while", "return", "def", "class", "int", "double", "void"
    };
    for (const auto& kw : keywords) {
        if (word == kw) return true;
    }
    return false;
}

bool Tokenizer::is_operator(char c) const {
    return c == '+' || c == '-' || c == '*' || c == '/' ||
           c == '(' || c == ')' || c == '{' || c == '}' ||
           c == '[' || c == ']' || c == ',' || c == ';' ||
           c == '=' || c == '<' || c == '>' || c == ':';
}

bool Tokenizer::is_whitespace(char c) const {
    return c == ' ' || c == '\t';
}

}  // namespace fungal
