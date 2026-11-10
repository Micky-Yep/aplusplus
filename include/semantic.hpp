#pragma once
#include "ast.hpp"

#include <string>
#include <unordered_set>
#include <vector>

class SemanticChecker {
public:
    void enterScope();
    void exitScope();
    void declareVariable(const std::string& name);
    void requireVariable(const std::string& name) const;
    void checkProgram(const std::vector<StmtPtr>& statements);

private:
    bool isDeclared(const std::string& name) const;
    void checkStatement(const Stmt& statement);
    void checkExpression(const Expr& expression) const;

    // Start with one scope for top-level variables.
    std::vector<std::unordered_set<std::string>> scopes_{1};
};