#include "lexer.h"

#include <stdexcept>

lexer::lexer(const std::string &text) {
    this->text = text;
}

char lexer::current() const { return position >= text.length() ? '\0' : text[position]; }

void lexer::advance() { position++; }

bool lexer::isAtEnd() const { return position >= text.length(); }

char lexer::peekChar() const {
    if (position + 1 >= text.length())
        return '\0';

    return text[position + 1];
}

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

token lexer::readString() {
    int start = position;
    advance();

    std::string sb;

    while (current() != '"' && current() != '\0') {
        if (current() == '\\') {
            advance();
            switch (current()) {
                case '"': sb.push_back('"'); break;
                case '\\': sb.push_back('\\'); break;
                case 'n':  sb.push_back('\n'); break;
                case 'r':  sb.push_back('\r'); break;
                case 't':  sb.push_back('\t'); break;
                case '0':  sb.push_back('\0'); break;
                default:
                    sb.push_back('\\');
                    sb.push_back(current());
                    break;
            }
        }
        else
        {
            sb.push_back(current());
        }
    }

    if (current() != '"')
        throw std::runtime_error("Unterminated string");

    advance();

    return token(StringLiteral, sb, start, position);
}

