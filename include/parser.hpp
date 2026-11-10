#pragma once 

#include "ast.hpp"

#include <initializer_list>
#include <string>
#include <vector>
#include <cstddef>

using namespace std;


class Parser {
public:
    explicit Parser(vector<Token> tokens);
    vector<StmtPtr> parse();

private:
    vector<Token> tokens_;
    vector<StmtPtr> block();
    size_t current_ = 0;

    StmtPtr declaration();
    StmtPtr variableDeclaration(bool isConst);
    StmtPtr statement();
    StmtPtr captureStatement();
    StmtPtr showStatement();
    StmtPtr assignmentOrExpressionStatement();
    StmtPtr expressionStatement();
    StmtPtr whenStatement();
    StmtPtr loopStatement();
    StmtPtr functionDeclaration();
    StmtPtr returnStatement();
    StmtPtr classDeclaration();

    ExprPtr expression();
    ExprPtr logicalOr();
    ExprPtr logicalAnd();
    ExprPtr equality();
    ExprPtr comparison();
    ExprPtr term();
    ExprPtr factor();
    ExprPtr unary();
    ExprPtr primary();

    bool match(initializer_list<TokenType> types);
    bool check(TokenType type) const;
    bool checkNext(TokenType type) const;
    bool isAtEnd() const;

    const Token& advance();
    const Token& peek() const;
    const Token& previous() const;
    Token consume(TokenType type, const string& message);

    [[noreturn]] void error(const Token& token, const string& message) const;
};