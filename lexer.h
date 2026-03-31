#pragma once
#include <string>
#include <vector>

#include "token.h"

class lexer {
    std::string text;
    int position;

    char current() const;
    void advance();

    token readNumber();
    token readIdentifier();
    char peekChar() const;
    bool isAtEnd() const;
    token readString();

public:
    lexer(const std::string& text);

    std::vector<token> tokenize();
};
