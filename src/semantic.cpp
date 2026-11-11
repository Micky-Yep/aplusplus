#include "semantic.hpp"

#include <stdexcept>

void SemanticChecker::enterScope() {
    scopes_.emplace_back();
}

void SemanticChecker::exitScope() {
    if (scopes_.size() > 1) {
        scopes_.pop_back();
    }
}

bool SemanticChecker::isDeclared(const std::string& name) const {
    for (auto scope = scopes_.rbegin(); scope != scopes_.rend(); ++scope) {
        if (scope->find(name) != scope->end()) {
            return true;
        }
    }
    return false;
}

void SemanticChecker::declareVariable(const std::string& name, bool isConst) {
    if (isDeclared(name)) {
        throw std::runtime_error("Variable already declared: " + name);
    }
    scopes_.back().emplace(name, isConst);
}

void SemanticChecker::requireVariable(const std::string& name) const {
    if (!isDeclared(name)) {
        throw std::runtime_error("Undeclared variable: " + name);
    }
}

void SemanticChecker::checkProgram(const std::vector<StmtPtr>& statements) {
    // Start each program with a fresh top-level scope.
    scopes_.clear();
    scopes_.emplace_back();

    for (const auto& statement : statements) {
        checkStatement(*statement);
    }
}

void SemanticChecker::checkStatement(const Stmt& statement) {
    if (const auto* decl = dynamic_cast<const VarDeclStmt*>(&statement)) {
        // Check the right-hand side before registering the new name.
        if (decl->initializer) {
            checkExpression(*decl->initializer);
        }
        declareVariable(decl->name.lexeme, decl->isConst);
        return;
    }

    if (const auto* show = dynamic_cast<const ShowStmt*>(&statement)) {
        for (const auto& value : show->values) {
            checkExpression(*value);
        }
        return;
    }
    if (const auto* assign = dynamic_cast<const AssignStmt*>(&statement)) {
        const auto& name = assign->name.lexeme;

        for (auto scope = scopes_.rbegin(); scope != scopes_.rend(); ++scope) {
            const auto entry = scope->find(name);
            if (entry == scope->end()) {
                continue;
            }

            if (entry->second) {
                throw std::runtime_error("Cannot assign to const variable: " + name);
            }

            checkExpression(*assign->value);
            return;
        }

        throw std::runtime_error("Undeclared variable: " + name);
    }
    throw std::runtime_error("Semantic checker: unsupported statement");
}

void SemanticChecker::checkExpression(const Expr& expression) const {
    if (dynamic_cast<const LiteralExpr*>(&expression)) {
        return;
    }

    if (const auto* variable = dynamic_cast<const VariableExpr*>(&expression)) {
        requireVariable(variable->name.lexeme);
        return;
    }

    if (const auto* unary = dynamic_cast<const UnaryExpr*>(&expression)) {
        checkExpression(*unary->right);
        return;
    }

    if (const auto* binary = dynamic_cast<const BinaryExpr*>(&expression)) {
        checkExpression(*binary->left);
        checkExpression(*binary->right);
        return;
    }

    throw std::runtime_error("Semantic checker: unsupported expression");
}