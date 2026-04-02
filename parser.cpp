#include "parser.h"

#include <stdexcept>

std::string tokenType_toString(const tokenType type) {
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
    if (current().type == type) {
        throw std::runtime_error("Expected " + tokenType_toString(type) + " but got " + tokenType_toString(current().type));
    }
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
    if (position + 1 >= tokens.size())
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