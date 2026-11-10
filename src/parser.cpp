#include "parser.hpp"

#include <stdexcept>
#include <utility>
#include <memory>

using namespace std;

Parser::Parser(vector<Token> tokens) : tokens_(std::move(tokens)) {}

bool Parser::match(initializer_list<TokenType> types) {
    for (TokenType type : types)
    {
        if (check(type))
        {
            advance();
            return true;
        }
        
    }
    return false;
}

bool Parser::check(TokenType type) const {
    if (isAtEnd())
    {
        return type == TokenType::EndOfFile;
    }
    return peek().type == type;
}

bool Parser::checkNext(TokenType type) const {
    size_t nextIndex = current_ + 1;
    return nextIndex < tokens_.size() && tokens_[nextIndex].type == type;
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
}

const Token& Parser::advance() {
    if (!isAtEnd())
    {
        ++current_;
        return previous();
    }
    return peek();
    
}

const Token& Parser::peek() const {
    return tokens_[current_];
}

const Token& Parser::previous() const {
    return tokens_[current_ - 1];
}

Token Parser::consume(TokenType type , const string& message) {
    if (check(type))
    {
        return advance();
    }
    error(peek(), message);
    
}

[[noreturn]] void Parser::error(const Token& token, const string& messages) const {
    throw runtime_error("Parse error at line " + to_string(token.line) + ", column " + to_string(token.column) + ": " + messages);
}

vector<StmtPtr> Parser:: parse() {
    vector<StmtPtr> statements;

    while (!isAtEnd())
    {
        statements.push_back(declaration());
    }
    return statements;
}

StmtPtr Parser::declaration() {
    if (match({TokenType::Let}))
    {
        return variableDeclaration(false);
    }
    
    if (match({TokenType::Const}))
    {
        return variableDeclaration(true);
    }

    if (match({TokenType::Capture}))
    {
        return captureStatement();
    }
    if (match({TokenType::Fn}))
    {
        return functionDeclaration();
    }
    if (match({TokenType::SetClass}))
    {
        return classDeclaration();
    }
    
    return statement();
    
}

StmtPtr Parser::variableDeclaration(bool isConst) {
    Token name = consume(TokenType:: Identifier, "Expected a variable name after let or const");

    consume(TokenType::Assign, "Expected '=' after the variable names");

    ExprPtr initializer = expression();

    consume(TokenType::Semicolon, "Expected ';' after the variable declaration");

    return make_unique<VarDeclStmt>(isConst, name, std::move(initializer));
}

StmtPtr Parser::captureStatement() {
    Token name = consume(TokenType::Identifier, "Expected a variable name after $capture");

    consume(TokenType:: In, "Expected 'in' after the variable name");

    if (!match({TokenType::NumberType, TokenType::TextType, TokenType::BoolType}))
    {
        error(peek(), "Expected a type: number, text, or bool.");
    }

    Token type = previous();

    consume(TokenType::Semicolon, "Expected ';' after the capture statement.");

    return make_unique<CaptureStmt>(name, type);
    
}

StmtPtr Parser::statement(){
    if (match({TokenType::Show}))
    {
        return showStatement();
    }
    if (match({TokenType::When}))
    {
        return whenStatement();
    }
    if (match({TokenType::Loop}))
    {
        return loopStatement();
    }
    if (match({TokenType::Skip}))
    {
        consume(TokenType::Semicolon, "Expected ';' after 'skip'");
        return make_unique<SkipStmt>();
    }
    if (match({TokenType::Stop}))
    {
        consume(TokenType::Semicolon, "Expected ';' after 'stop'");
        return make_unique<StopStmt>();
    }
    if (match({TokenType::Return}))
    {
        return returnStatement();
    }
    return assignmentOrExpressionStatement();
}

StmtPtr Parser::showStatement() {
    vector<ExprPtr> values;
    values.push_back(expression());

    while (match({TokenType::Comma}))
    {
        values.push_back(expression());
    }
    
    consume(TokenType::Semicolon, "Expected ';' after the show statement");
    return make_unique<ShowStmt>(std::move(values));
}

StmtPtr Parser::assignmentOrExpressionStatement() {
    if (check(TokenType::Identifier) && checkNext(TokenType::Assign))
    {
        Token name = advance();
        consume(TokenType ::Assign, "Expected '=' after the variable name");
        ExprPtr value = expression();
        consume(TokenType::Semicolon, "Expected ';' after the assignment");

        return make_unique<AssignStmt>(name, std::move(value));
    }
    return expressionStatement();
}

StmtPtr Parser::expressionStatement(){
    ExprPtr value = expression();
    consume(TokenType::Semicolon, "Expected ';' after the expression");
    return make_unique<ExprStmt>(std::move(value));
}

ExprPtr Parser::expression() {
    return logicalOr();
}

ExprPtr Parser::logicalOr() {
    ExprPtr expr = logicalAnd();
    while (match({TokenType::Or}))
    {
        Token op = previous();
        ExprPtr right = logicalAnd();
        expr = make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprPtr Parser::logicalAnd() {
    ExprPtr expr = equality();

    while (match({TokenType::And}))
    {
        Token op = previous();
        ExprPtr right = equality();
        expr = make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
    
}

ExprPtr Parser::equality() {
    ExprPtr expr = comparison();
    while (match({TokenType::Equal, TokenType::NotEqual}))
    {
        Token op = previous();
        ExprPtr right = comparison();
        expr = make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
    
}

ExprPtr Parser::comparison() {
    ExprPtr expr = term();

    while (match({TokenType::Less, TokenType::LessEqual, TokenType::Greater, TokenType::GreaterEqual}))
    {
        Token op = previous();
        ExprPtr right = term();
        expr = make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
    
}

ExprPtr Parser::term() {
    ExprPtr expr = factor();
    while (match({TokenType::Plus, TokenType::Minus}))
    {
        Token op = previous();
        ExprPtr right = factor();
        expr = make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprPtr Parser::factor() {
    ExprPtr expr = unary();
    while (match({TokenType::Multiply, TokenType::Divide}))
    {
        Token op = previous();
        ExprPtr right = unary();
        expr = make_unique<BinaryExpr>(std::move(expr), op , std::move(right));
    }
    return expr;
}

ExprPtr Parser::unary() {
    if (match({TokenType::Not, TokenType::Minus}))
    {
        Token op = previous();
        ExprPtr right = unary();
        return make_unique<UnaryExpr>(op, std::move(right));
    }
    return primary();
    
}

ExprPtr Parser::primary() {
    ExprPtr expr; 
    if (match({TokenType::NumberLiteral, TokenType::TextLiteral, TokenType::True, TokenType::False}))
    {
        expr = make_unique<LiteralExpr>(previous());
    } else if (match({TokenType::Spawn}))
    {
        Token className = consume(TokenType::Identifier, "Expected a class name after 'spawn'");
        consume(TokenType::LeftBracket, "Expected '[' after the class name");

        vector<SpawnField> fields;

        if(!check (TokenType::RightBracket))
        {
            do
            {
                Token fieldName = consume(TokenType::Identifier, "Expected a field name");
                consume(TokenType::Assign, "Expected '=' after the field name");
                ExprPtr value = expression();
                fields.emplace_back(fieldName, std::move(value));
            } while (match({TokenType::Comma}));
        }
        consume(TokenType::RightBracket, "Expected ']' after the field initializers");
        expr = make_unique<SpawnExpr>(className, std::move(fields));
    } else if (match({TokenType::Self}))
    {
        expr = make_unique<VariableExpr>(previous());
    }else if (match({TokenType::Identifier}))
    {
        Token name = previous();
        if (match({TokenType::LeftParen}))
        {
            vector<ExprPtr> arguments;
            if (!check(TokenType::RightParen))
            {
                do
                {
                    arguments.push_back(expression());
                } while (match({TokenType::Comma}));
                
            }
            consume(TokenType::RightParen, "Expected ')' after function arguments");
            expr = make_unique<CallExpr>(name, std::move(arguments));
        } else {
            expr = make_unique<VariableExpr>(name);
        }
        
    } else if (match({TokenType::LeftBracket})) 
    {
        vector<ExprPtr> elements;

        if (!check(TokenType::RightBracket))
        {
            do
            {
                elements.push_back(expression());
            } while (match({TokenType::Comma}));
            
        }
        
        consume(TokenType::RightBracket, "Expected ']' after array elements");
        expr = make_unique<ArrayExpr>(std::move(elements));
    } else if (match({TokenType::LeftParen}))
    {
        expr = expression();
        consume(TokenType::RightParen, "Expected ')' after the expression");
    } else {
        error(peek(), "Expected an expression");
    }

    while (true)
    {
        if (match({TokenType::LeftBracket}))
        {
            ExprPtr index = expression();
            consume(TokenType::RightBracket, "Expected ']' after the array index");
            expr = make_unique<IndexExpr>(std::move(expr), std::move(index));
        } else if (match({TokenType::Dot}))
        {
            Token member = consume(TokenType::Identifier, "Expected a member name after '.'");
            if (match({TokenType::LeftParen}))
            {
                vector<ExprPtr> arguments;
                if (!check(TokenType::RightParen))
                {
                    do
                    {
                        arguments.push_back(expression());
                    } while (match({TokenType::Comma}));
                    
                }
                consume(TokenType::RightParen, "Expected ')' after method arguments");
                expr = make_unique<MethodCallExpr>(std::move(expr), member, std::move(arguments));
            } else {
                expr = make_unique<MemberExpr>(std::move(expr), member);
            }
            
        } else {
            break;
        }
        
        
    }
    
    return expr;
    
}

vector<StmtPtr> Parser::block() {
    vector<StmtPtr> statements;

    while(!check(TokenType::RightBrace) && !isAtEnd()) {
        statements.push_back(declaration());
    }

    consume(TokenType::RightBrace, "Expected '}' after the block");
    return statements;
}

StmtPtr Parser::whenStatement(){
    vector<WhenBranch> branches;

    do{
        consume(TokenType::LeftParen, "Expected '(' after 'when'");
        ExprPtr condition = expression();
        consume(TokenType::RightParen, "Expected ')' after the condition");
        consume(TokenType::LeftBrace, "Expected '{' after the condition");
      

        vector<StmtPtr> body = block();
        branches.emplace_back(std::move(condition), std::move(body));
    } while (match({TokenType::When}));
    consume(TokenType::Default, "Expected 'default' after the when branches");
    consume(TokenType::LeftBrace, "Expected '{' before the default block");

    vector<StmtPtr> defaultBody = block();
    return make_unique<WhenStmt>(std::move(branches), std::move(defaultBody));
    
}

StmtPtr Parser::loopStatement() {
    consume(TokenType::LeftBrace, "Expected '{' after 'loop'");
    vector<StmtPtr> body = block();

    consume(TokenType::Until, "expected 'until' after the loop body");
    consume(TokenType::LeftParen, "Expected '(' after 'until'");
    ExprPtr condition = expression();
    consume(TokenType::RightParen, "Expected ')' after the loop condition");
    consume(TokenType::Semicolon, "Expected ';' after the loop");

    return make_unique<LoopStmt>(std::move(body), std::move(condition));
}

StmtPtr Parser::functionDeclaration() {
    Token name = consume(TokenType::Identifier, "Expected a function name after 'fn'");
    consume(TokenType::LeftParen, "Expected '(' after the function name");
    vector<FunctionParameter> parameters;
    if (!check(TokenType::RightParen))
    {
        do
        {
            if (!match({TokenType::NumberType, TokenType::TextType, TokenType::BoolType}))
            {
                error(peek(), "Expected a parameter type: number, text, or bool");
            }
            
            Token type = previous();
            bool isArray = false;

            if (match({TokenType::LeftBracket}))
            {
                consume(TokenType::RightBracket, "Expected ']' after '[' in the parameter type");
                isArray = true;
            }
            
            consume(TokenType::Colon, "Expected ':' after the parameter type");
            Token parameterName = consume(TokenType::Identifier, "Expected a parameter name");
            parameters.push_back({type, parameterName, isArray});
        } while (match({TokenType::Comma}));
        
    }
    consume(TokenType::RightParen, "Expected ')' after the parameters");
    consume(TokenType::Arrow, "Expected '->' before the return type");
    if (!match({TokenType::NumberType, TokenType::TextType, TokenType::BoolType}))
    {
        error(peek(), "Expected a return type : number, text, or bool");
    }

    Token returnType = previous();
    bool returnsArray = false;

    if (match({TokenType::LeftBracket}))
    {
        consume(TokenType::RightBracket, "Expected ']' after '[' in the return type");
        returnsArray = true;
    }
    
    consume(TokenType::LeftBrace, "Expected '{' before the function body");
    vector<StmtPtr> body = block();
    return make_unique<FunctionDeclStmt>(name, std::move(parameters), returnType, returnsArray, std::move(body));
    
    
}

StmtPtr Parser::classDeclaration() {
    Token name = consume(TokenType::Identifier, "Expected a class name after 'setclass");
    consume(TokenType::Start, "Expected 'start' after the class name");

    vector<ClassMember> members;

    while (!check(TokenType::Colon) && !isAtEnd())
    {
        if (!match({TokenType::Public, TokenType::Protected, TokenType::Private}))
        {
            error(peek(), "Expected public, protected, or private before a class member");
        }
        Token access = previous();

        StmtPtr member;
        if (match({TokenType::Let}))
        {
            member = variableDeclaration(false);
        } else if (match({TokenType::Const})) {
            member = variableDeclaration(true);
        } else if (match({TokenType::Fn}))
        {
            member = functionDeclaration();
        } else {
            error(peek(), "Expected let, const, or fn after the access modifier");
        }
        members.emplace_back(access, std::move(member));
        
    }
    consume(TokenType::Colon, "Expected ':' before 'end'");
    consume(TokenType::End, "Expected 'end' after the class members");
    match({TokenType::Semicolon});
    return make_unique<ClassDeclStmt>(name, std::move(members));
}

StmtPtr Parser::returnStatement() {
    ExprPtr value = expression();
    consume(TokenType::Semicolon, "Expected ';' after the return value");
    return make_unique<ReturnStmt>(std::move(value));
}