#include "lexer.h"

#include <stdexcept>
#include <unordered_map>

char lexer::current() const { return position >= text.length() ? '\0' : text[position]; }

void lexer::advance() { position++; }

token lexer::readNumber() {
    int start = position;
    bool hasDot = false;

    while (!isAtEnd() && (std::isdigit(current()) || current() == '.')) {
        if (current() == '.') {
            if (hasDot)
                throw std::runtime_error("Invalid number format");

            hasDot = true;
        }

        advance();
    }

    std::string text = text.substr(start, position - start);

    if (hasDot)
        return token(FloatLiteral, text, start, position);
    else
        return token(IntLiteral, text, start, position);
}

token lexer::readIdentifier() {
    int start = position;
    std::string sb;

    while (std::isalnum(current()) || current() == '_') {
        sb += current();
        advance();
    }

    static const std::unordered_map<std::string, tokenType> keywords = {
        {"bool", BoolKeyword},
        {"true", True},
        {"false", False},
        {"if", If},
        {"else", Else},
        {"while", While},
        {"int", IntKeyword},
        {"float", FloatKeyword},
        {"string", StringKeyword},
        {"void", VoidKeyword},
        {"return", Return},
        {"for", For},
        {"foreach", Foreach},
        {"in", In},
        {"struct", StructKeyword},
        {"enum", Enum},
        {"class", Class},
        {"interface", Interface},
        {"new", New},
        {"override", Override},
        {"super", Super},
        {"switch", Switch},
        {"case", Case},
        {"default", Default},
        {"public", Public},
        {"private", Private},
        {"protected", Protected},
        {"import", Import}
    };

    auto it = keywords.find(sb);

    tokenType type = (it != keywords.end()) ? it->second : Identifier;

    return token(type, sb, start, position);
}

char lexer::peekChar() const {
    if (position + 1 >= text.length())
        return '\0';

    return text[position + 1];
}

bool lexer::isAtEnd() const { return position >= text.length(); }

token lexer::readString() {
    int start = position;
    advance();

    std::string sb;

    while (current() != '"' && current() != '\0') {
        if (current() == '\\') {
            advance();
            switch (current()) {
                case '"': sb.push_back('"');
                    break;
                case '\\': sb.push_back('\\');
                    break;
                case 'n': sb.push_back('\n');
                    break;
                case 'r': sb.push_back('\r');
                    break;
                case 't': sb.push_back('\t');
                    break;
                case '0': sb.push_back('\0');
                    break;
                default:
                    sb.push_back('\\');
                    sb.push_back(current());
                    break;
            }
        } else {
            sb.push_back(current());
        }
    }

    if (current() != '"')
        throw std::runtime_error("Unterminated string");

    advance();

    return token(StringLiteral, sb, start, position);
}

lexer::lexer(const std::string &text) {
    this->text = text;
}

std::vector<token> lexer::tokenize() {
    std::vector<token> tokens;

    while (current() != '\0') {
        if (std::isspace(current())) {
            advance();
            continue;
        }

        int start = position;

        if (std::isdigit(current())) {
            tokens.push_back(readNumber());
            continue;
        }

        if (std::isalpha(current())) {
            tokens.push_back(readIdentifier());
            continue;
        }

        if (current() == '"') {
            tokens.push_back(readString());
            continue;
        }

        switch (current()) {
            case '+':
                tokens.emplace_back(tokenType::Plus, "+", start, start + 1);
                break;
            case '-':
                tokens.emplace_back(tokenType::Minus, "-", start, start + 1);
                break;
            case '*':
                tokens.emplace_back(tokenType::Star, "*", start, start + 1);
                break;
            case '/':
                if (peekChar() == '/') {
                    while (current() != '\n' && current() != '\r' && current() != '\0') {
                        advance();
                    }
                    continue;
                }
                tokens.emplace_back(tokenType::Slash, "/", start, start + 1);
                break;
            case '(':
                tokens.emplace_back(tokenType::LParen, "(", start, start + 1);
                break;
            case ')':
                tokens.emplace_back(tokenType::RParen, ")", start, start + 1);
                break;
            case ';':
                tokens.emplace_back(tokenType::Semicolon, ";", start, start + 1);
                break;
            case '{':
                tokens.emplace_back(tokenType::LBrace, "{", start, start + 1);
                break;
            case '}':
                tokens.emplace_back(tokenType::RBrace, "}", start, start + 1);
                break;
            case '=':
                if (peekChar() == '=') {
                    advance();
                    tokens.emplace_back(tokenType::EqualEqual, "==", start, position + 1);
                } else {
                    tokens.emplace_back(tokenType::Equal, "=", start, start + 1);
                }
                break;
            case '>':
                if (peekChar() == '=') {
                    advance();
                    tokens.emplace_back(tokenType::GreaterEqual, ">=", start, position + 1);
                } else {
                    tokens.emplace_back(tokenType::Greater, ">", start, start + 1);
                }
                break;
            case '<':
                if (peekChar() == '=') {
                    advance();
                    tokens.emplace_back(tokenType::LessEqual, "<=", start, position + 1);
                } else {
                    tokens.emplace_back(tokenType::Less, "<", start, start + 1);
                }
                break;
            case '&':
                if (peekChar() == '&') {
                    advance();
                    tokens.emplace_back(tokenType::AndAnd, "&&", start, position + 1);
                } else {
                    throw std::runtime_error("Unexpected character '&'");
                }
                break;
            case '|':
                if (peekChar() == '|') {
                    advance();
                    tokens.emplace_back(tokenType::OrOr, "||", start, position + 1);
                } else {
                    throw std::runtime_error("Unexpected character '|'");
                }
                break;
            case '!':
                if (peekChar() == '=') {
                    advance();
                    tokens.emplace_back(tokenType::NotEqual, "!=", start, position + 1);
                } else {
                    tokens.emplace_back(tokenType::Bang, "!", start, start + 1);
                }
                break;
            case ',':
                tokens.emplace_back(tokenType::Comma, ",", start, start + 1);
                break;
            case ':':
                if (peekChar() == ':') {
                    advance();
                    tokens.emplace_back(tokenType::ColonColon, "::", start, position + 1);
                } else {
                    tokens.emplace_back(tokenType::Colon, ":", start, start + 1);
                }
                break;
            case '.':
                tokens.emplace_back(tokenType::Dot, ".", start, start + 1);
                break;
            case '[':
                tokens.emplace_back(tokenType::LBracket, "[", start, start + 1);
                break;
            case ']':
                tokens.emplace_back(tokenType::RBracket, "]", start, start + 1);
                break;
        }

        advance();
    }

    tokens.emplace_back(tokenType::EndOfFile, "", position, position);
    return tokens;
}