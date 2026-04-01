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

token parser::Expect(tokenType type) {
    if (current().type == type) {
        throw std::runtime_error("Expected " + tokenType_toString(type) + " but got " + tokenType_toString(current().type));
    }
}
