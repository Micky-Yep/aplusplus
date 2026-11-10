#pragma once 

#include <cstddef>
#include <string>

using namespace std;

enum class TokenType {
    Identifier,
    NumberLiteral,
    TextLiteral,
    True,
    False,

    Let,
    Const,
    Fn,
    Return,
    Show,
    Capture,
    In,
    NumberType,
    TextType,
    BoolType,

    When,
    Default,
    Loop,
    Until,
    Skip,
    Stop,

    SetClass,
    Extends,
    Start,
    End,
    Public,
    Protected,
    Private,
    Self,
    Spawn,
    Override,

    Plus,
    Minus,
    Multiply,
    Divide,
    Assign,
    Equal,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    And,
    Or,
    Not,
    Arrow,

    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    LeftBracket,
    RightBracket,
    Colon,
    Comma,
    Semicolon,
    Dot,
    
    EndOfFile
};

struct Token {
    TokenType type;
    string lexeme;
    size_t line;
    size_t column;
};

