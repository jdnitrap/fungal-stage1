#include "../../src/layer1/tokenizer.hpp"
#include <gtest/gtest.h>

using namespace fungal;

TEST(TokenizerTest, TokenizeSimpleCode) {
    Tokenizer tokenizer;
    std::string code = "if (x > 5) { return y; }";
    auto tokens = tokenizer.tokenize(code);

    EXPECT_GT(tokens.size(), 0);
    EXPECT_EQ(tokens[0].value, "if");
    EXPECT_EQ(tokens[0].type, "keyword");
}

TEST(TokenizerTest, IdentifyOperators) {
    Tokenizer tokenizer;
    std::string code = "a + b";
    auto tokens = tokenizer.tokenize(code);

    bool found_operator = false;
    for (const auto& tok : tokens) {
        if (tok.type == "operator") {
            found_operator = true;
            break;
        }
    }
    EXPECT_TRUE(found_operator);
}

TEST(TokenizerTest, EmptyCode) {
    Tokenizer tokenizer;
    auto tokens = tokenizer.tokenize("");
    EXPECT_EQ(tokens.size(), 0);
}
