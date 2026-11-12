#include "codegen.hpp"
#include <sstream>      // Used like a text-building workshop to assemble pieces of code together
#include <stdexcept>    // Used for throwing errors if something goes wrong

using namespace std;

// This function translates A++ data types into regular C++ data types.
// Think of it as translating vocabulary from one language to another.
string CodeGenerator::mapType(const string& aplusType) const {
    if (aplusType == "number") return "double";       // A++ 'number' becomes C++ 'double' (decimals/integers)
    if (aplusType == "text") return "std::string";   // A++ 'text' becomes C++ 'std::string' (sentences/words)
    if (aplusType == "bool") return "bool";          // A++ 'bool' stays 'bool' (true or false)
    return "auto";                                   // Fallback type if unknown
}

// This is the starting point of code generation. It sets up the required C++ libraries
// and loops through every statement in our program to translate them.
string CodeGenerator::generate(const vector<StmtPtr>& statements) {
    stringstream generatedCode; // A string builder to collect our translated C++ code

    // Automatically add standard C++ libraries to the top of our output file
    generatedCode << "#include <iostream>\n";
    generatedCode << "#include <string>\n";
    generatedCode << "#include <vector>\n\n";

    // Translate every single statement found in the syntax tree
    for (const auto& stmt : statements) {
        generatedCode << generateStatement(*stmt) << "\n";
    }

    // Convert our text builder into a normal string and return it
    return generatedCode.str();
}

// This function looks at what kind of statement we have (like a variable, a print command, or a loop)
// and writes the matching C++ code for it.
string CodeGenerator::generateStatement(const Stmt& statement) {
    
    // 1. Handling Variable Declarations (e.g., let score = 72.5;)
    if (const auto* decl = dynamic_cast<const VarDeclStmt*>(&statement)) {
        string mutability = decl->isConst ? "const " : ""; // If it's a constant, mark it 'const' in C++
        string init = decl->initializer ? " = " + generateExpression(*decl->initializer) : "";
        return mutability + "auto " + decl->name.lexeme + init + ";";
    }

    // 2. Handling Assignments (e.g., score = 80;)
    if (const auto* assign = dynamic_cast<const AssignStmt*>(&statement)) {
        return assign->name.lexeme + " = " + generateExpression(*assign->value) + ";";
    }

    // 3. Handling Show/Print statements (e.g., show "Score:", score;)
    if (const auto* show = dynamic_cast<const ShowStmt*>(&statement)) {
        stringstream ss;
        ss << "std::cout"; // Standard C++ print command
        for (size_t i = 0; i < show->values.size(); ++i) {
            ss << " << " << generateExpression(*show->values[i]);
            // If there are multiple items, print a space between them
            if (i + 1 < show->values.size()) {
                ss << " << \" \"";
            }
        }
        ss << " << std::endl;"; // End the line with a newline
        return ss.str();
    }

    // 4. Handling regular expression statements
    if (const auto* exprStmt = dynamic_cast<const ExprStmt*>(&statement)) {
        return generateExpression(*exprStmt->expression) + ";";
    }

    // 5. Handling Return statements inside functions (e.g., return left + right;)
    if (const auto* ret = dynamic_cast<const ReturnStmt*>(&statement)) {
        return "return " + (ret->value ? generateExpression(*ret->value) : "") + ";";
    }

    // 6. Handling Function Declarations (e.g., fn add(...) -> number { ... })
    if (const auto* func = dynamic_cast<const FunctionDeclStmt*>(&statement)) {
        stringstream ss;
        // Write the return type and function name
        ss << mapType(func->returnType.lexeme) << " " << func->name.lexeme << "(";
        
        // Write out all parameters
        for (size_t i = 0; i < func->parameters.size(); ++i) {
            ss << mapType(func->parameters[i].type.lexeme) << " " << func->parameters[i].name.lexeme;
            if (i + 1 < func->parameters.size()) ss << ", ";
        }
        ss << ") {\n";
        
        // Translate every statement inside the function body
        for (const auto& stmt : func->body) {
            ss << "    " << generateStatement(*stmt) << "\n";
        }
        ss << "}\n";
        return ss.str();
    }

    // 7. Handling Loops (e.g., loop { ... } until (condition))
    if (const auto* loop = dynamic_cast<const LoopStmt*>(&statement)) {
        stringstream ss;
        ss << "while (true) {\n"; // Translate 'loop' into an infinite C++ while-loop
        for (const auto& stmt : loop->body) {
            ss << "    " << generateStatement(*stmt) << "\n";
        }
        // Translate the 'until' condition into a break condition
        ss << "    if (" << generateExpression(*loop->condition) << ") { break; }\n";
        ss << "}\n";
        return ss.str();
    }

    // 8. Handling Skip (continues to next loop iteration)
    if (dynamic_cast<const SkipStmt*>(&statement)) {
        return "continue;";
    }

    // 9. Handling Stop (breaks out of a loop)
    if (dynamic_cast<const StopStmt*>(&statement)) {
        return "break;";
    }

    throw runtime_error("Code generator: unsupported statement type");
}

// This function translates expressions—the parts of code that calculate values or hold data
string CodeGenerator::generateExpression(const Expr& expression) {
    
    // Literal values (like numbers or text strings)
    if (const auto* lit = dynamic_cast<const LiteralExpr*>(&expression)) {
        return lit->value.lexeme;
    }

    // Variable names (like 'score' or 'total')
    if (const auto* var = dynamic_cast<const VariableExpr*>(&expression)) {
        return var->name.lexeme;
    }

    // Unary operations (like negative numbers, e.g., -5)
    if (const auto* unary = dynamic_cast<const UnaryExpr*>(&expression)) {
        return unary->op.lexeme + generateExpression(*unary->right);
    }

    // Binary operations (math and logic, like a + b or x >= 5)
    if (const auto* binary = dynamic_cast<const BinaryExpr*>(&expression)) {
        string op = binary->op.lexeme;
        return "(" + generateExpression(*binary->left) + " " + op + " " + generateExpression(*binary->right) + ")";
    }

    // Function calls (like add(3, 4))
    if (const auto* call = dynamic_cast<const CallExpr*>(&expression)) {
        stringstream ss;
        ss << call->callee.lexeme << "(";
        for (size_t i = 0; i < call->arguments.size(); ++i) {
            ss << generateExpression(*call->arguments[i]);
            if (i + 1 < call->arguments.size()) ss << ", ";
        }
        ss << ")";
        return ss.str();
    }

    throw runtime_error("Code generator: unsupported expression type");
}