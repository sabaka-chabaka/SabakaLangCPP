#pragma once
#include <string>
#include <vector>
#include <concepts>
#include <optional>

#include "ast.h"
#include "token.h"

class expr;
typedef std::string string;

class parser {
    std::vector<token> tokens;
    int position = 0;

public:
    parser(const std::vector<token> &tokens);

    token current();
    token consume();

private:
    template <typename T>
    requires std::derived_from<T, expr>
    T* withPos(T* expr, const token& start, const token& end) {
        expr->Start = start.tokenStart;
        expr->End = end.tokenEnd;
        return expr;
    }

    template <typename T>
    requires std::derived_from<T, expr>
    T* withPos(T* expr, int start, int end) {
        expr->Start = start;
        expr->End = end;
        return expr;
    }

    template <typename T>
    requires std::derived_from<T, expr>
    T* withPos(T* expr, const token& token) {
        expr->Start = token.tokenStart;
        expr->End = token.tokenEnd;
        return expr;
    }

    token expect(tokenType type);

    std::pair<tokenType, string> consumeTypeFull();
    tokenType consumeType();
    std::vector<string> tryParseTypeArgs();
    std::vector<string> tryParseTypeParams();

    static bool isTypeKeyword(tokenType type);
    static bool isAccessModifier(tokenType type);

    static AccessModifier getAccessModifier(tokenType type);

    token peek(int offset = 1);

    bool isFunctionDeclaration();
    bool isVariableDeclaration();
};