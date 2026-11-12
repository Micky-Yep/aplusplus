#pragma once // Ensures this header file is only included once during compilation

#include "ast.hpp"     // Brings in the Abstract Syntax Tree definitions (expressions, statements)
#include <string>      // Allows the use of text strings
#include <vector>      // Allows the use of lists (vectors) of statements

// The CodeGenerator class translates our custom A++ language into standard C++ code
class CodeGenerator {
public:
    // Main public function: takes a list of A++ statements and returns a single C++ text string
    std::string generate(const std::vector<StmtPtr>& statements);

private:
    // Helper function: converts a single A++ statement into its C++ equivalent code
    std::string generateStatement(const Stmt& stmt);

    // Helper function: converts an A++ expression (like math or variables) into C++ code
    std::string generateExpression(const Expr& expr);

    // Helper function: maps A++ data types (like number, text) to C++ types (double, std::string)
    std::string mapType(const std::string& aplusType) const;
};