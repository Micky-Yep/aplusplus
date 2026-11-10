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

void SemanticChecker::declareVariable(const std::string& name) {
    // Reject redeclaration and shadowing, following the 2a design.
    if (isDeclared(name)) {
        throw std::runtime_error("Variable already declared: " + name);
    }
    scopes_.back().insert(name);
}

void SemanticChecker::requireVariable(const std::string& name) const {
    if (!isDeclared(name)) {
        throw std::runtime_error("Undeclared variable: " + name);
    }
}