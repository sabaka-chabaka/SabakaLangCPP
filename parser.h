#pragma once]

#include "ast.h"
#include <string>
#include <vector>
#include <type_traits>
#include <optional>
#include "token.h"

enum AccessModifier;
typedef std::string string;

class parser {
    std::vector<token> tokens;
    int position = 0;

public:
    parser(const std::vector<token> &tokens);

    token current();
    token consume();

private:
    template <typename T, std::enable_if_t<std::is_base_of_v<expr, T>, int> = 0>
    T* withPos(T* node, const token& start, const token& end) {
        node->start = start.tokenStart;
        node->end = end.tokenEnd;
        return node;
    }

    template <typename T, std::enable_if_t<std::is_base_of_v<expr, T>, int> = 0>
    T* withPos(T* node, int start, int end) {
        node->start = start;
        node->end = end;
        return node;
    }

    template <typename T, std::enable_if_t<std::is_base_of_v<expr, T>, int> = 0>
    T* withPos(T* node, const token& token) {
        node->start = token.tokenStart;
        node->end = token.tokenEnd;
        return node;
    }

    std::string tokenType_toString(tokenType type);

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

public:
    std::vector<expr*> parseProgram();

private:
    expr* parseVariableDeclaration();
    expr* parseBlockOrStatement();
    expr* parseStatement();
    expr* parseIf();
    expr* parseSwitch();
    expr* parseBlock();
    expr* parseAssignment();
    expr* ParseComparsion();
    expr* parseAdditive();
    expr* parseMultiplicative();
    expr* parseUnary();
    expr* parsePrimary();
    expr* parseWhile();
    expr* ParseLogicalOr();
    expr* ParseLogicalAnd();
    expr* parseFunction();
    expr* parseParameters();
    expr* parseReturn();
    expr* parseFor();
    expr* parseForeach();
    expr* parseStruct();
    expr* parseEnum();
    expr* parseClass();
    expr* parseInterface();
    expr* parseImport();
};