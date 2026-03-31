#pragma once
#include <string>

enum tokenType {
    Number,

    Plus,
    Minus,
    Star,
    Slash,
    Percent,

    LParen,
    RParen,

    Semicolon,
    Identifier,

    BoolKeyword,
    True,
    False,
    Equal,

    If,
    Else,
    While,
    For,
    LBrace,
    RBrace,
    Enum,

    EqualEqual,
    NotEqual,
    Greater,
    Less,
    GreaterEqual,
    LessEqual,

    IntLiteral,
    FloatLiteral,
    IntKeyword,
    FloatKeyword,
    StringLiteral,
    StringKeyword,

    Function,
    Return,
    VoidKeyword,
    Comma,

    Foreach,
    In,
    Dot,
    StructKeyword,

    AndAnd,
    OrOr,
    Bang,

    LBracket,
    RBracket,

    Class,
    New,
    Override,
    Colon,
    ColonColon,
    Super,
    Interface,

    Switch,
    Case,
    Default,

    Comment,

    Public,
    Private,
    Protected,

    Import,

    EndOfFile
};

class token {
public:
    tokenType type;
    std::string value;
    int tokenStart;
    int tokenEnd;

    token(const tokenType type, const std::string& value, int start, int end);
    std::string toString();
};