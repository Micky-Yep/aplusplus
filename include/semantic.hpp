#pragma once
#include "ast.hpp"

#include <string>
#include <unordered_map>
#include <vector>

class SemanticChecker {
public:
    void enterScope();
    void exitScope();
    void declareVariable(const std::string& name, bool isConst = false);
    void requireVariable(const std::string& name) const;
    void checkProgram(const std::vector<StmtPtr>& statements);

private:
    bool isDeclared(const std::string& name) const;
    void checkStatement(const Stmt& statement);

    // Each scope maps a variable name to whether it is constant.
    enum class ValueType { Unknown, Number, Text, Bool };
    ValueType checkExpression(const Expr& expression) const;

    struct Symbol {
        bool isConst;
        ValueType type;
    };

    std::vector<std::unordered_map<std::string, Symbol>> scopes_{1};
};