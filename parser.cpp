#include "parser.h"

#include <stdexcept>

std::string parser::tokenType_toString(const tokenType type) {
    switch (type) {
        case Number: return "Number";

        case Plus: return "Plus";
        case Minus: return "Minus";
        case Star: return "Star";
        case Slash: return "Slash";
        case Percent: return "Percent";

        case LParen: return "LParen";
        case RParen: return "RParen";

        case Semicolon: return "Semicolon";
        case Identifier: return "Identifier";

        case BoolKeyword: return "BoolKeyword";
        case True: return "True";
        case False: return "False";
        case Equal: return "Equal";

        case If: return "If";
        case Else: return "Else";
        case While: return "While";
        case For: return "For";
        case LBrace: return "LBrace";
        case RBrace: return "RBrace";
        case Enum: return "Enum";

        case EqualEqual: return "EqualEqual";
        case NotEqual: return "NotEqual";
        case Greater: return "Greater";
        case Less: return "Less";
        case GreaterEqual: return "GreaterEqual";
        case LessEqual: return "LessEqual";

        case IntLiteral: return "IntLiteral";
        case FloatLiteral: return "FloatLiteral";
        case IntKeyword: return "IntKeyword";
        case FloatKeyword: return "FloatKeyword";
        case StringLiteral: return "StringLiteral";
        case StringKeyword: return "StringKeyword";

        case Function: return "Function";
        case Return: return "Return";
        case VoidKeyword: return "VoidKeyword";
        case Comma: return "Comma";

        case Foreach: return "Foreach";
        case In: return "In";
        case Dot: return "Dot";
        case StructKeyword: return "StructKeyword";

        case AndAnd: return "AndAnd";
        case OrOr: return "OrOr";
        case Bang: return "Bang";

        case LBracket: return "LBracket";
        case RBracket: return "RBracket";

        case Class: return "Class";
        case New: return "New";
        case Override: return "Override";
        case Colon: return "Colon";
        case ColonColon: return "ColonColon";
        case Super: return "Super";
        case Interface: return "Interface";

        case Switch: return "Switch";
        case Case: return "Case";
        case Default: return "Default";

        case Comment: return "Comment";

        case Public: return "Public";
        case Private: return "Private";
        case Protected: return "Protected";

        case Import: return "Import";
        case This: return "This";

        case EndOfFile: return "EndOfFile";
    }

    return "Unknown";
}

parser::parser(const std::vector<token> &tokens) {
    this->tokens = tokens;
}

token parser::current() {
    if (position >= tokens.size()) {
        return tokens.back();
    }
    return tokens[position];
}

token parser::consume() {
    token cur = current();
    position++;
    return cur;
}

token parser::expect(tokenType type) {
    if (current().type != type) {
        throw std::runtime_error("Expected " + tokenType_toString(type) + " but got " + tokenType_toString(current().type));
    }

    return consume();
}

std::pair<tokenType, string> parser::consumeTypeFull() {
    token token1 = consume();
    tokenType type = token1.type;
    std::string customType;

    if (type == Identifier) {
        customType = token1.value;
    }

    if (type == Identifier && current().type == Less) {
        std::vector<string> typeArgs = tryParseTypeArgs();
        if (!typeArgs.empty()) {
            string joinedArgs;
            for (const auto& arg : typeArgs) {
                joinedArgs += "$" + arg;
            }
            customType = customType + joinedArgs;
        }
    }

    while (current().type == LBracket) {
        consume();
        expect(RBracket);
    }

    return { type, customType };
}

tokenType parser::consumeType() {
    return consumeTypeFull().first;
}

bool parser::isTypeKeyword(tokenType type) {
    return type == IntKeyword || type == FloatKeyword || type == BoolKeyword || type == StringKeyword || type == VoidKeyword;
}

bool parser::isAccessModifier(tokenType type) {
    return type == Public || type == Private || type == Protected;
}

AccessModifier parser::getAccessModifier(tokenType type) {
    switch (type) {
        case Public: return PublicAc;
        case Private: return PrivateAc;
        case Protected: return ProtectedAc;
        default: return PublicAc;
    }
}

std::vector<string> parser::tryParseTypeArgs() {
    std::vector<string> result;
    if (current().type != Less) return result;

    int saved = position;
    consume();

    while (true) {
        if (!isTypeKeyword(current().type) && current().type != Identifier) {
            position = saved;
            return {};
        }
        string typeName = current().value;
        consume();

        while (current().type == LBracket && peek().type == RBracket) {
            typeName += "[]";
            consume(); consume();
        }
        result.push_back(typeName);
        if (current().type == Comma) {consume(); continue;}
        break;
    }

    if (current().type != Greater) {
        position = saved;
        return {};
    }
    consume();
    return result;
}

token parser::peek(int offset) {
    if (position + offset >= tokens.size())
        return tokens.back();

    return tokens[position + offset];
}

std::vector<string> parser::tryParseTypeParams() {
    std::vector<string> result;
    if (current().type != Less) return result;

    int saved = position;
    consume();

    while (true) {
        if (current().type != Identifier) {
            position = saved;
            return {};
        }

        result.push_back(consume().value);
        if (current().type == Comma) {consume(); continue;}
        break;
    }

    if (current().type != Greater) {
        position = saved;
        return {};
    }
    consume();
    return result;
}

bool parser::isFunctionDeclaration() {
    int offset = 0;
    if (isAccessModifier(peek(offset).type)) offset++;

    if (peek(offset).type == Override) offset++;

    if (!isTypeKeyword(peek(offset).type) && peek(offset).type != Identifier) return false;

    offset++;

    if (peek(offset).type == Less) {
        offset++;
        int depth = 1;
        while (depth > 0 && position + offset < tokens.size()) {
            tokenType t = peek(offset).type;
            if (t == Less) depth++;
            else if (t == Greater) depth--;
            else if (t == Semicolon || t == EndOfFile) break;
            offset++;
        }
        if (depth != 0 ) return false;
    }

    while (peek(offset).type == LBracket) {
        if (peek(offset + 1).type == RBracket) return false;
        offset += 2;
    }

    if (peek(offset).type == Less) {
        offset++;
        int depth = 1;
        while (depth > 0 && position + offset < tokens.size()) {
            tokenType t = peek(offset).type;
            if (t == Less) depth++;
            else if (t == Greater) depth--;
            else if (t == Semicolon || t == EndOfFile) break;
            offset++;
        }
        if (depth != 0 ) return false;
    }

    return peek(offset).type == LParen;
}

bool parser::isVariableDeclaration() {
    int offset = 0;
    if (isAccessModifier(peek(offset).type)) offset++;

    if (isTypeKeyword(peek(offset).type) && peek(offset).type != VoidKeyword) return true;

    if (peek(offset).type == Identifier) {
        offset++;

        if (peek(offset).type == Less) {
            int saved = offset;
            offset++;
            int depth = 1;
            while (depth > 0 && position + offset < tokens.size()) {
                tokenType t = peek(offset).type;
                if (t == Less) depth++;
                else if (t == Greater) depth--;
                else if (t == Semicolon || t == EndOfFile) break;
                offset++;
            }
            if (depth != 0) {
                return false;
            }

            return peek(offset).type == Identifier;
        }

        while (peek(offset).type == LBracket) {
            if (peek(offset + 1).type != RBracket) return false;
            offset += 2;
        }

        return peek(offset).type == Identifier;
    }

    return false;
}

std::vector<expr*> parser::parseProgram() {
    std::vector<expr*> expressions;

    while (current().type != EndOfFile) {
        expr* exp = nullptr;

        if (isFunctionDeclaration()) {
            exp = parseFunction();
        }
        else {
            exp = parseStatement();
        }

        if (exp)
            expressions.push_back(exp);
    }

    return expressions;
}

expr *parser::parseVariableDeclaration(AccessModifier defaultAccess) {
    token startToken = current();
    AccessModifier access = defaultAccess;
    if (isAccessModifier(current().type)) {
        access = getAccessModifier(current().type);
    }

    std::pair<tokenType, string> typeFull = consumeTypeFull();

    token nameToken = expect(Identifier);
    expr* initializer = nullptr;

    if (current().type == Equal) {
        consume();
        initializer = parseAssignment();
    }

    return withPos(new variableDeclaration(typeFull.first, typeFull.second, nameToken.value, initializer, access), startToken, tokens[position - 1]);
}

std::vector<expr*> parser::parseBlockOrStatement() {
    if (current().type == LBrace)
        return parseBlock();

    expr* statement = parseStatement();
    return { statement };
}

expr* parser::parseStatement() {
    expr* stmt;
    if (current().type == If) stmt = parseIf();
    else if (current().type == Switch) stmt = parseSwitch();
    else if (isVariableDeclaration()) {
        stmt = parseVariableDeclaration();
        if (current().type == Semicolon) consume();
        return stmt;
    }
    else if (current().type == While) stmt = parseWhile();
    else if (current().type == Return) stmt = parseReturn();
    else if (current().type == Foreach) stmt = parseForeach();
    else if (current().type == For) stmt = parseFor();
    else if (current().type == StructKeyword) stmt = parseStruct();
    else if (current().type == Import) stmt = parseImport();
    else if (current().type == Class) stmt = parseClass();
    else if (current().type == Interface) stmt = parseInterface();
    else if (current().type == Enum) stmt = parseEnum();
    else stmt = parseAssignment();

    if (current().type == Semicolon) consume();
    return stmt;
}

expr* parser::parseIf() {
    token startToken = consume();
    expect(LParen);
    expr* condition = parseAssignment();
    expect(RParen);
    std::vector<expr*> thenBlock = parseBlockOrStatement();

    std::vector<expr*> elseBlock;
    if (current().type == Else) {
        consume();
        elseBlock = parseBlockOrStatement();
    }

    return withPos(new ifStatement(condition, thenBlock, elseBlock), startToken, tokens[position - 1]);
}

expr* parser::parseSwitch() {
    token startToken = consume();
    expect(LParen);
    expr* expression = parseAssignment();
    expect(RParen);
    std::vector<switchCase*> cases;
    bool hasDefault = false;

    while (current().type != RBrace && current().type != EndOfFile) {
        if (current().type == Case) {
            consume();
            expr* value = parseAssignment();
            expect(Colon);
            std::vector<expr*> body = parseBlockOrStatement();
            cases.push_back(new switchCase(value, body));
        }
        else if (current().type == Default) {
            if (hasDefault) throw std::runtime_error("Multiple default cases in switch");

            consume();
            hasDefault = true;
            expect(Colon);
            std::vector<expr*> body = parseBlockOrStatement();
            cases.push_back(new switchCase(nullptr, body));
        }
        else {
            throw std::runtime_error("Expected 'case' or 'default' got " + tokenType_toString(current().type) + " instead. In switch " + startToken.value);
        }
    }

    expect(RBrace);
    return withPos(new switchStatement(expression, cases), startToken, tokens[position - 1]);
}

std::vector<expr*> parser::parseBlock() {
    expect(LBrace);

    std::vector<expr*> statements;

    while (current().type != RBrace && current().type != EndOfFile) {
        statements.push_back(parseStatement());
    }

    expect(RBrace);
    return statements;
}

expr* parser::parseAssignment() {
    token startToken = current();
    if (current().type == Super) {
        if (peek().type == ColonColon && peek(2).type == Identifier && peek(3).type == Equal) {
            token sToken = consume();
            consume();
            string member = consume().value;
            consume();
            expr* value = parseAssignment();
            return withPos(new memberAssignmentExpr(withPos(new superExpr(), sToken), member, value), startToken, tokens[position - 1]);
        }
    }

    if (current().type == Identifier) {
        if (peek().type == Equal) {
            string name = consume().value;
            consume();
            expr* value = parseAssignment();
            return withPos(new assignmentExpr(&name, value), startToken, tokens[position - 1]);
        }

        if (peek().type == LBracket) {
            int offset = 2;
            int depth = 1;
            while (position + offset < tokens.size() && depth > 0) {
                if (tokens[position + offset].type == LBracket) depth++;
                else if (tokens[position + offset].type == RBracket) depth--;
                offset++;
            }

            if (position + offset < tokens.size() && tokens[position + offset].type == Equal) {
                token identifier = consume();
                consume();
                expr* index = parseAssignment();
                expect(RBracket);
                expect(Equal);
                expr* value = parseAssignment();
                return withPos(new arrayStoreExpr(withPos(new variableExpr(identifier.value), identifier), index, value), startToken, tokens[position - 1]);
            }
        }

        if (peek().type == Dot) {
            int offset = 1;
            while (peek(offset).type == Dot && peek(offset + 1).type == Identifier) offset += 2;

            if (peek(offset).type == LBracket) {
                int savedOffset = offset;
                offset++;
                int depth = 1;
                while (position + offset < tokens.size() && depth > 0) {
                    if (tokens[position + offset].type == LBracket) depth++;
                    else if (tokens[position + offset].type == RBracket) depth--;
                    offset++;
                }
                if (position + offset < tokens.size() && tokens[position + offset].type == Equal) {
                    token firstToken = current();
                    expr* obj = withPos(new variableExpr(consume().value), firstToken);
                    while (current().type == Dot) {
                        consume();
                        token memberToken = expect(Identifier);
                        obj = withPos(new memberAccessExpr(obj, memberToken.value), firstToken, tokens[position - 1]);
                    }
                    expect(LBracket);
                    expr* index = parseAssignment();
                    expect(RBracket);
                    expect(Equal);
                    expr* value = parseAssignment();
                    return withPos(new arrayStoreExpr(obj, index, value), startToken, tokens[position - 1]);
                }
            }

            if (peek(offset).type == Equal) {
                token startToken = current();
                expr* obj = withPos(new variableExpr(consume().value), startToken);

                while (current().type == Dot && peek(2).type == Identifier && peek(3).type != Equal) {
                    consume();
                    token memberToken = expect(Identifier);
                    obj = withPos(new memberAccessExpr(obj, memberToken.value), startToken, tokens[position - 1]);
                }

                consume();
                token memberToken = expect(Identifier);
                expect(Equal);
                expr* value = parseAssignment();
                return withPos(new memberAssignmentExpr(obj, memberToken.value, value), startToken, tokens[position - 1]);
            }
        }
    }

    return parseLogicalOr();
}

expr* parser::parseComparsion() {
    expr* left = parseAdditive();
    while (current().type == EqualEqual || current().type == NotEqual || current().type == Greater || current().type == Less || current().type == GreaterEqual || current().type == LessEqual) {
        token opToken = consume();
        expr* right = parseAdditive();
        left = withPos(new binaryExpr(left, opToken.type, right), left->start, right->end);
    }

    return left;
}

expr* parser::parseAdditive() {
    expr* exp = parseMultiplicative();
    while (current().type == Plus || current().type == Minus) {
        token opToken = consume();
        expr* right = parseMultiplicative();
        exp = withPos(new binaryExpr(exp, opToken.type, right), exp->start, right->end);
    }

    return exp;
}

expr* parser::parseMultiplicative() {
    expr* exp = parseUnary();
    while (current().type == Star || current().type == Slash || current().type == Percent) {
        token opToken = consume();
        expr* right = parseUnary();

        exp = withPos(new binaryExpr(exp, opToken.type, right), exp->start, right->end);
    }

    return exp;
}

expr* parser::parseUnary() {
    token startToken = current();
    if (current().type == Minus) {
        tokenType op = consume().type;
        expr* exp = parseUnary();
        return withPos(new unaryExpr(op, exp), startToken, tokens[position - 1]);
    }

    if (current().type == Bang) {
        tokenType op = consume().type;
        expr* exp = parseUnary();
        return withPos(new unaryExpr(op, exp), startToken, tokens[position - 1]);
    }

    return parsePrimary();
}

expr* parser::parsePrimary() {
    expr* exp;
    token startToken = current();

    if (current().type == New) {
        consume();
        string name = expect(Identifier).value;
        std::vector<string> typeArgs = tryParseTypeArgs();
        expect(LParen);

        std::vector<expr*> args;
        if (current().type != RParen) {
            do {
                args.push_back(parseAssignment());
                if (current().type != Comma) break;
                consume();
            } while (true);
        }
        expect(RParen);

        exp = withPos(new newExpr(name, typeArgs, args), startToken, tokens[position - 1]);
    }
    else if (current().type == IntLiteral) {
        exp = withPos(new intExpr(std::stoi(current().value)), current());
        consume()''
    }
    else if (current().type == FloatLiteral) {
        exp = withPos(new floatExpr(std::stod(current().value)), current());
        consume();
    }
    else if (current().type == StringLiteral) {
        exp = withPos(new stringExpr(current().value), current());
    }
    else if (current().type == True) {
        exp = withPos(new boolExpr(true), current());
        consume();
    }
    else if (current().type == False) {
        exp = withPos(new boolExpr(false), current());
        consume();
    }
    else if (current().type == Identifier) {
        exp = withPos(new variableExpr(current().value), current());
        consume();
    }
    else if (current().type == This) {
        exp = withPos(new thisExpr(), current());
        consume();
    }
    else if (current().type == Super) {
        exp = withPos(new superExpr(), current());
        //TODO
    }

    return exp;
}

expr* parser::parseWhile() {
    token startToken = consume();

    expect(LParen);
    expr* condition = parseAssignment();
    expect(RParen);

    std::vector<expr*> body = parseBlock();

    return withPos(new whileExpr(condition, body), startToken, tokens[position - 1]);
}

expr* parser::parseLogicalOr() {
    expr* left = parseLogicalAnd();
    while (current().type == OrOr) {
        token opToken = consume();
        expr* right = parseLogicalAnd();
        left = withPos(new binaryExpr(left, opToken.type, right), left->start, right->end);
    }

    return left;
}

expr* parser::parseLogicalAnd() {
    expr* left = parseComparsion();

    while (current().type == AndAnd) {
        token opToken = consume();
        expr* right = parseComparsion();
        left = withPos(new binaryExpr(left, opToken.type, right), left->start, right->end);
    }

    return left;
}

expr* parser::parseFunction(AccessModifier defaultAccess) {
    token startToken = current();
    AccessModifier access = defaultAccess;
    if (isAccessModifier(current().type)) {
        access = getAccessModifier(consume().type);
    }

    bool isOverride = false;
    if (current().type == Override) {
        isOverride = true;
        consume();
    }

    tokenType returnType = consumeType();
    string name = expect(Identifier).value;

    std::vector<string> typeParams = tryParseTypeParams();

    expect(LParen);
    std::vector<parameter*> parameters = parseParameters();
    expect(RParen);

    std::vector<expr*> body = parseBlock();

    return withPos(new functionDeclaration(returnType, name, typeParams, &parameters, body, access), startToken, tokens[position - 1]);
}

std::vector<parameter*> parser::parseParameters() {
    std::vector<parameter*> parameters;

    if (current().type != RParen) {
        do {
            std::pair<tokenType, string> typeFull = consumeTypeFull();

            string paramName = expect(Identifier).value;

            parameters.push_back(new parameter(typeFull.first, typeFull.second, paramName));

            if (current().type != Comma) break;

            consume();
        } while (true);
    }

    return parameters;
}

expr* parser::parseReturn() {
    token startToken = consume();
    if (current().type == Semicolon) return withPos(new returnStatement(nullptr), startToken, tokens[position - 1]);

    expr* value = parseAssignment();
    return withPos(new returnStatement(value), startToken, tokens[position - 1]);
}

expr* parser::parseFor() {
    token startToken = consume();
    expect(LParen);

    expr* initializer = nullptr;
    if (current().type != Semicolon) initializer = parseAssignment();
    else consume();

    expr* condition = nullptr;
    if (current().type != Semicolon) condition = parseAssignment();
    else consume();

    expect(Semicolon);

    expr* increment = nullptr;
    if (current().type != RParen) increment = parseAssignment();

    expect(RParen);

    std::vector<expr*> body = parseBlockOrStatement();

    return withPos(new forStatement(initializer, condition, increment, body), startToken, tokens[position - 1]);
}

expr* parser::parseForeach() {
    token startToken = consume();

    expect(LParen);

    consumeType();

    string variableName = expect(Identifier).value;

    expect(In);

    expr* collection = parseAssignment();

    expect(RParen);

    std::vector<expr*> body = parseBlock();

    return withPos(new foreachStatement(variableName, collection, body), startToken, tokens[position - 1]);
}

expr* parser::parseStruct() {
    token startToken = consume();
    string name = expect(Identifier).value;

    expect(LBrace);

    std::vector<string> fields;

    while (current().type != RBrace) {
        consumeType();
        string fieldName = expect(Identifier).value;
        expect(Semicolon);

        fields.push_back(fieldName);
    }

    expect(RBrace);

    return withPos(new structDeclaration(name, fields), startToken, tokens[position - 1]);
}

expr* parser::parseEnum() {
    token startToken = consume();
    string name = expect(Identifier).value;

    expect(LBrace);

    std::vector<string> members;

    while (current().type != RBrace) {
        string valueName = expect(Identifier).value;
        members.push_back(valueName);
        if (current().type != Comma) consume();
    }

    expect(RBrace);

    return withPos(new enumDeclaration(name, members), startToken, tokens[position - 1]);
}


expr* parser::parseImport() {
    token startToken = consume();
    const string path = expect(StringLiteral).value;

    if (current().type == Semicolon) consume();

    return withPos(new importStatement(filePath), startToken, tokens[position - 1]);
}

expr* parser::parseInterface() {
    token startToken = consume();
    string name = expect(Identifier).value;
    std::vector<string> typeParams = tryParseTypeParams();

    std::vector<string> parents;
    if (current().type == Colon) {
        consume();
        do {
            parents.push_back(expect(Identifier).value);
            if (current().type != Comma) break;
            consume();
        } while (true);
    }

    expect(LBrace);

    std::vector<functionDeclaration*> methods;

    while (current().type != RBrace) {
        token methodStartToken = current();
        tokenType returnType = consumeType();
        string methodName = expect(Identifier).value;

        expect(LParen);
        std::vector<parameter*> parameters = parseParameters();
        expect(RParen);
        expect(Semicolon);

        methods.push_back(withPos(new functionDeclaration(returnType, methodName, typeParams, &parameters, {}, AccessModifier::Public), methodStartToken, tokens[position - 1]));
    }

    expect(RBrace);

    return withPos(new interfaceDeclaration(name, typeParams, parents, methods), startToken, tokens[position - 1]);
}

expr* parser::parseClass() {
    token startToken = consume();
    string name = expect(Identifier).value;
    std::vector<string> typeParams = tryParseTypeParams();

    string baseClassName;
    std::vector<string> interfaces;

    if (current().type == Colon) {
        consume();
        string first = expect(Identifier).value;
        baseClassName = first;
        while (current().type == Comma) {
            consume();
            interfaces.push_back(expect(Identifier).value);
        }
    }

    expect(LBrace);

    std::vector<variableDeclaration*> fields;
    std::vector<functionDeclaration*> methods;

    while (current().type != RBrace) {
        if (isFunctionDeclaration()) {
            methods.push_back(dynamic_cast<functionDeclaration*>(parseFunction()));
        }
        else if (isVariableDeclaration()) {
            variableDeclaration* field = dynamic_cast<variableDeclaration*>(parseVariableDeclaration());
            expect(Semicolon);
            fields.push_back(field);
        }
    }

    expect(RBrace);

    return withPos(new classDeclaration(name, typeParams, baseClassName, interfaces, fields, methods), startToken, tokens[position - 1]);
}