#pragma once 

#include "token.hpp"
#include <memory>
#include <vector>
#include <utility>

using namespace std;
struct Expr {
    virtual ~Expr() = default;
};

using ExprPtr = unique_ptr<Expr>;

struct LiteralExpr : Expr {
    Token value;

    explicit LiteralExpr(const Token& token) : value(token) {};
};

struct VariableExpr : Expr {
    Token name;

    explicit VariableExpr(const Token& token) : name(token) {}
};

struct UnaryExpr : Expr {
    Token op;
    ExprPtr right;

    UnaryExpr(const Token& operation, ExprPtr operand) : op(operation), right(std::move(operand)) {}
};

struct BinaryExpr : Expr {
    ExprPtr left;
    Token op;
    ExprPtr right;

    BinaryExpr(ExprPtr lhs, const Token& operation, ExprPtr rhs) : left(std::move(lhs)), op(operation), right(std::move(rhs)){}
};

struct Stmt {
    virtual ~Stmt() = default;
};

using StmtPtr = unique_ptr<Stmt>;

struct VarDeclStmt : Stmt {
    bool isConst;
    Token name;
    ExprPtr initializer;

    VarDeclStmt(bool constant, const Token& variableName, ExprPtr value) : isConst(constant), name(variableName), initializer(std::move(value)) {}
};

struct AssignStmt : Stmt {
    Token name;
    ExprPtr value;

    AssignStmt(const Token& variableName, ExprPtr newValue) : name(variableName), value(std::move(newValue)){}
};

struct ShowStmt : Stmt {
    vector<ExprPtr> values;

    explicit ShowStmt(vector<ExprPtr> items) : values(std::move(items)) {}
};

struct CaptureStmt : Stmt {
    Token name;
    Token type; 

    CaptureStmt(const Token& variableName, const Token& variableType) : name(variableName), type(variableType) {}
};

struct ExprStmt : Stmt {
    ExprPtr expression;

    explicit ExprStmt(ExprPtr value) : expression(std::move(value)) {}
};

struct WhenBranch {
    ExprPtr condition;
    vector<StmtPtr> body;

    WhenBranch(ExprPtr test, vector<StmtPtr> statements) : condition(std::move(test)), body(std::move(statements)) {}
};

struct WhenStmt : Stmt {
    vector<WhenBranch> branches;
    vector<StmtPtr> defaultBody;

    WhenStmt(vector<WhenBranch> cases, vector<StmtPtr> fallback) : branches(std::move(cases)), defaultBody(std::move(fallback)) {}
};

struct LoopStmt : Stmt {
    vector<StmtPtr> body;
    ExprPtr condition;

    LoopStmt(vector<StmtPtr> statements, ExprPtr test) : body(std::move(statements)), condition(std::move(test)) {}
};

struct SkipStmt : Stmt {};
struct StopStmt : Stmt {};

struct FunctionParameter {
    Token type;
    Token name;
    bool isArray = false;
};

struct FunctionDeclStmt : Stmt {
    Token name;
    vector<FunctionParameter> parameters;
    Token returnType;
    bool returnsArray = false;
    vector<StmtPtr> body;

    FunctionDeclStmt(const Token& functionName, vector<FunctionParameter> functionParameters, const Token& resultType, bool arrayResult, vector<StmtPtr> statements) : name(functionName),parameters(std::move(functionParameters)), returnType(resultType), returnsArray(arrayResult), body(std::move(statements)){}

};

struct ReturnStmt : Stmt {
    ExprPtr value;
    explicit ReturnStmt(ExprPtr result) : value(std::move(result)) {}
};

struct CallExpr : Expr {
    Token callee;
    vector<ExprPtr> arguments;

    CallExpr(const Token& functionName, vector<ExprPtr> args) : callee(functionName), arguments(std::move(args)) {}
}; 

struct ArrayExpr : Expr {
    vector<ExprPtr> elements;
    explicit ArrayExpr(vector<ExprPtr> values) : elements(std::move(values)){}
};

struct IndexExpr : Expr {
    ExprPtr array;
    ExprPtr index;

    IndexExpr(ExprPtr target, ExprPtr position) : array(std::move(target)), index(std::move(position)) {}
};

struct MemberExpr : Expr {
    ExprPtr object;
    Token member;

    MemberExpr(ExprPtr target, const Token& memberName) : object(std::move(target)), member(memberName) {}
};

struct ClassMember {
    Token access;
    StmtPtr declaration;

    ClassMember(const Token& visibility, StmtPtr member) : access(visibility), declaration(std::move(member)) {}
};

struct ClassDeclStmt : Stmt {
    Token name;
    vector<ClassMember> members;

    ClassDeclStmt(const Token& className, vector<ClassMember> classMembers) : name(className), members(std::move(classMembers)) {}
};

struct SpawnField {
    Token name;
    ExprPtr value;
    
    SpawnField(const Token& fieldName, ExprPtr fieldValue) : name(fieldName), value(std::move(fieldValue)) {}
};

struct SpawnExpr : Expr {
    Token className;
    vector<SpawnField> fields;

    SpawnExpr(const Token& name, vector<SpawnField> initializers) : className(name), fields(std::move(initializers)) {}
};

struct MethodCallExpr : Expr {
    ExprPtr object;
    Token method;
    vector<ExprPtr> arguments;

    MethodCallExpr(ExprPtr target, const Token& methodName, vector<ExprPtr> args) : object(std::move(target)), method(methodName),arguments(std::move(args)) {}
}; 