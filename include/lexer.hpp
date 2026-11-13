#pragma once 
#include <cstddef>
#include <string>
#include <vector>
#include "token.hpp"

using namespace std ;
class Lexer {
public: 
    explicit Lexer(string source);
    vector<Token> scanTokens();

private:
    string source_;
    vector<Token> tokens_;
    size_t start_ = 0;
    size_t current_ = 0;
    size_t line_ = 1;
    size_t column_ = 1;
    size_t startLine_ = 1;
    size_t startColumn_ = 1;

    bool isAtEnd() const;
    char advance();
    char peek() const;
    char peekNext() const;
    bool match(char expected);

    void scanToken();
    void scanNumber();
    void scanText();
    void scanIdentifier();
    void skipWhitespaceAndComments();
    void addToken(TokenType type);
    void error(const string& message) const;
};