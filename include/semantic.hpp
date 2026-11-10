#pragma once

#include <string>
#include <unordered_set>
#include <vector>

class SemanticChecker {
public:
    void enterScope();
    void exitScope();
    void declareVariable(const std::string& name);
    void requireVariable(const std::string& name) const;

private:
    bool isDeclared(const std::string& name) const;

    // Start with one scope for top-level variables.
    std::vector<std::unordered_set<std::string>> scopes_{1};
};