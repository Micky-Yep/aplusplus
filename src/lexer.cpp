#include "lexer.hpp"
#include <utility>
#include <stdexcept>
#include <cctype>
#include <unordered_map>

using namespace std;

Lexer::Lexer(string source) : source_(std::move(source)) {}

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

void Lexer::error(const string& message) const {
    throw runtime_error("Line " + to_string(startLine_) +
                        ", column " + to_string(startColumn_) +
                        ": " + message);
}


vector<Token> Lexer::scanTokens() {
    
    // Reset before scanning.
    tokens_.clear();
    start_ = 0;
    current_ = 0;
    line_ = 1;
    column_ = 1;
    startLine_ = 1;
    startColumn_ = 1;

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

void Lexer::scanToken() {
    char character = advance();

    switch (character)
    {
    case '(':
        addToken(TokenType::LeftParen);
        break;
    case ')':
        addToken(TokenType::RightParen);
        break;
    case '{':
        addToken(TokenType::LeftBrace);
        break;    
    case '}':
        addToken(TokenType::RightBrace);
        break;
    case '[':
        addToken(TokenType::LeftBracket);
        break;
    case ']':
        addToken(TokenType::RightBracket);
        break;
    case ':':
        addToken(TokenType::Colon);
        break;
    case ',':
        addToken(TokenType::Comma);
        break;
    case ';':
        addToken(TokenType::Semicolon);
        break;
    case '.':
        addToken(TokenType::Dot);
        break;
    case '+':
        addToken(TokenType::Plus);
        break;
    case '*':
        addToken(TokenType::Multiply);
        break;
    case '/':
        addToken(TokenType::Divide);
        break;
    
    case '-':
        addToken(match('>') ? TokenType::Arrow : TokenType::Minus);
        break;
    case '!':
        addToken(match('=') ? TokenType::NotEqual : TokenType::Not);
        break;
    case '=':
        addToken(match('=') ? TokenType::Equal : TokenType::Assign);
        break;
    case '<':
        addToken(match('=') ? TokenType::LessEqual : TokenType::Less);
        break;
    case '>':
        addToken(match('=') ? TokenType::GreaterEqual : TokenType::Greater);
        break;

    case '&':
        if (match('&'))
        {
            addToken(TokenType::And);
        }else {
            error("Use && for the AND operator");
        }
        break;
    
    case '|':
        if (match('|'))
        {
            addToken(TokenType::Or);
        } else {
            error("Use || for the OR operator");
        }
        break;
    
    case '$': {
        bool validCapture = source_.compare(start_, 8, "$capture") == 0;
        if (validCapture && start_ + 8 < source_.size())
        {
            char next = source_[start_ + 8];
            bool continuesName = (next >= 'a' && next <= 'z') || (next >= 'A' && next <= 'Z') || (next >= '0' && next <= '9') || next == '_';
            if (continuesName)
            {
                validCapture = false;
            }
            
        }
        
        if (!validCapture)
        {
            error("Expected keyword $capture");
        }

        while (current_ < start_ + 8)
        {
            advance();
        }
        
        addToken(TokenType::Capture);
        break;
    }

    case '"':
        scanText();
        break;

    default:
        if (character >= '0' && character <= '9')
        {
            scanNumber();
        } else if ((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') || character == '_') {
            scanIdentifier();
        } else {
            error("Unexpected character");
        }

    }
}

void Lexer::scanNumber() {
    while (isdigit(static_cast<unsigned char>(peek())))
    {
        advance();
    }

    if (peek() == '.' && isdigit(static_cast<unsigned char>(peekNext())))
    {
        advance();

        while (isdigit(static_cast<unsigned char>(peek())))
        {
            advance();
        }
        
    }
    
    addToken(TokenType::NumberLiteral);
}

void Lexer::scanText() {
    while(!isAtEnd() && peek() != '"') {
        if (peek() == '\n')
        {
            error("Text literals cannot span multiple lines");
        }

        advance();
    
    }
    if (isAtEnd())
    {
        error("Unterminated text literal");
    }

    advance();
    addToken(TokenType::TextLiteral);
    
}

void Lexer::scanIdentifier() {
    while (isalnum(static_cast<unsigned char>(peek())) || peek() == '_')
    {
        advance();
    }
    
    string text = source_.substr(start_, current_ - start_);

    static const unordered_map<string, TokenType> keywords = {
        {"true", TokenType::True},
        {"false", TokenType::False},
        {"let", TokenType::Let},
        {"const", TokenType::Const},
        {"fn", TokenType::Fn},
        {"return", TokenType::Return},
        {"show", TokenType::Show},
        {"in", TokenType::In},
        {"number", TokenType::NumberType},
        {"text", TokenType::TextType},
        {"bool", TokenType::BoolType},
        {"when", TokenType::When},
        {"default", TokenType::Default},
        {"loop", TokenType::Loop},
        {"until", TokenType::Until},
        {"skip", TokenType::Skip},
        {"stop", TokenType::Stop},
        {"setclass", TokenType::SetClass},
        {"extends", TokenType::Extends},
        {"start", TokenType::Start},
        {"end", TokenType::End},
        {"public", TokenType::Public},
        {"protected", TokenType::Protected},
        {"private", TokenType::Private},
        {"self", TokenType::Self},
        {"spawn", TokenType::Spawn},
        {"override", TokenType::Override},
    };

    auto keyword = keywords.find(text);

    if (keyword != keywords.end())
    {
        addToken(keyword->second);
    } else {
        addToken(TokenType::Identifier);
    }
    
}