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
    scopes_.back().emplace(name, Symbol{isConst, ValueType::Unknown});
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
        const auto type = decl->initializer
            ? checkExpression(*decl->initializer)
            : ValueType::Unknown;

        declareVariable(decl->name.lexeme, decl->isConst);
        scopes_.back().at(decl->name.lexeme).type = type;
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

            if (entry->second.isConst) {
                throw std::runtime_error("Cannot assign to const variable: " + name);
            }

            const auto type = checkExpression(*assign->value);
            if (entry->second.type != ValueType::Unknown &&
                type != entry->second.type) {
                throw std::runtime_error("Assignment changes variable type: " + name);
            }
            return;
        }

        throw std::runtime_error("Undeclared variable: " + name);
    }

    if (const auto* when = dynamic_cast<const WhenStmt*>(&statement)) {
        auto checkBlock = [this](const std::vector<StmtPtr>& body) {
            enterScope();
            for (const auto& stmt : body) {
                checkStatement(*stmt);
            }
            exitScope();
        };

        for (const auto& branch : when->branches) {
            if (checkExpression(*branch.condition) != ValueType::Bool) {
                throw std::runtime_error("When condition must be Bool");
            }
            checkBlock(branch.body);
        }

        checkBlock(when->defaultBody);
        return;
    }
    throw std::runtime_error("Semantic checker: unsupported statement");
}

SemanticChecker::ValueType
SemanticChecker::checkExpression(const Expr& expression) const {
    if (const auto* literal = dynamic_cast<const LiteralExpr*>(&expression)) {
        switch (literal->value.type) {
            case TokenType::NumberLiteral: return ValueType::Number;
            case TokenType::TextLiteral: return ValueType::Text;
            case TokenType::True:
            case TokenType::False: return ValueType::Bool;
            default: return ValueType::Unknown;
        }
    }



     if (const auto* variable = dynamic_cast<const VariableExpr*>(&expression)) {
        const auto& name = variable->name.lexeme;
        for (auto scope = scopes_.rbegin(); scope != scopes_.rend(); ++scope) {
            const auto entry = scope->find(name);
            if (entry != scope->end()) {
                return entry->second.type;
            }
        }
        throw std::runtime_error("Undeclared variable: " + name);
    }

    if (const auto* unary = dynamic_cast<const UnaryExpr*>(&expression)) {
        checkExpression(*unary->right);
        return unary->op.type == TokenType::Not
            ? ValueType::Bool
            : ValueType::Number;
    }

    if (const auto* binary = dynamic_cast<const BinaryExpr*>(&expression)) {
        const auto left = checkExpression(*binary->left);
        const auto right = checkExpression(*binary->right);

        switch (binary->op.type) {
            case TokenType::Equal:
            case TokenType::NotEqual:
            case TokenType::Less:
            case TokenType::LessEqual:
            case TokenType::Greater:
            case TokenType::GreaterEqual:
            case TokenType::And:
            case TokenType::Or:
                return ValueType::Bool;
            case TokenType::Plus:
                return left == ValueType::Text || right == ValueType::Text
                    ? ValueType::Text
                    : ValueType::Number;
            case TokenType::Minus:
            case TokenType::Multiply:
            case TokenType::Divide:
                return ValueType::Number;
            default:
                return ValueType::Unknown;
        }
    }

    throw std::runtime_error("Semantic checker: unsupported expression");
}