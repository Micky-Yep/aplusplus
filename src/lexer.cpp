#include "lexer.hpp"
#include <utility>
#include <stdexcept>

using namespace std;

Lexer::Lexer(string source) : source_(move(source)) {}

bool Lexer::isAtEnd() const {
    return current_ >= source_.size();
}

char Lexer::advance() {
    char character = source_[current_++];

    if (character == '\n')
    {
        ++line_;
        column_ = 1;
    } else {
        ++column_;
    }
    return character;
}

char Lexer::peek() const {
    if (isAtEnd())
    {
        return '\0';
    }
    return source_[current_];
    
}

char Lexer::peekNext() const {
    if (current_ + 1 >= source_.size())
    {
        return '\0';
    }
    return source_[current_ + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd() || source_[current_] != expected)
    {
        return false;
    }

    advance();
    return true;
    
}

void Lexer::addToken(TokenType type) {
    string lexeme = source_.substr(start_, current_ - start_);
    tokens_.push_back({type, lexeme, startLine_, startColumn_});
}

vector<Token> Lexer::scanTokens() {
    while (!isAtEnd())  
    {
        skipWhitespaceAndComments();

        if (isAtEnd())
        {
            break;
        }
        
        start_ = current_;
        startLine_ = line_;
        startColumn_ = column_;

        scanToken();
    }
    
    tokens_.push_back({TokenType::EndOfFile, "", line_, column_});
    return tokens_;
}

void Lexer::skipWhitespaceAndComments() {
    while (!isAtEnd())
    {
        char character = peek();

        if (character == ' ' || character == '\r' || character == '\t' || character == '\n')
        {
            advance();
        } else if (character == '/' && peekNext() == '/')
        {
            while (!isAtEnd() && peek() != '\n')
            {
                advance();
            }
            
        } else {
            break;
        }
        
        
    }
    
}