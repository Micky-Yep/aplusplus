#include "codegen.hpp"
#include <sstream>
#include <stdexcept>

using namespace std;

string CodeGenerator::mapType(const string& aplusType) const {
    if (aplusType == "number") return "double";
    if (aplusType == "text") return "std::string";
    if (aplusType == "bool") return "bool";
    return "auto";
}

string CodeGenerator::generate(const vector<StmtPtr>& statements) {
    // changed by L - We now split the code into three parts: Headers, Globals, and Main!
    stringstream includes, globals, mainBody;
    
    includes << "#include <iostream>\n";
    includes << "#include <string>\n";
    includes << "#include <vector>\n\n";

    // changed by L - Open the required C++ int main() block for execution code
    mainBody << "int main() {\n";

    for (const auto& stmt : statements) {
        // changed by L - Functions and Classes must go outside main() in C++
        if (dynamic_cast<const FunctionDeclStmt*>(stmt.get()) || 
            dynamic_cast<const ClassDeclStmt*>(stmt.get())) {
            globals << generateStatement(*stmt) << "\n";
        } 
        // changed by L - Normal statements (loops, math, variables) go inside main()
        else {
            mainBody << "    " << generateStatement(*stmt) << "\n";
        }
    }

    // changed by L - Close the main block
    mainBody << "    return 0;\n}\n";

    // Combine them all seamlessly!
    return includes.str() + globals.str() + mainBody.str();
}

string CodeGenerator::generateStatement(const Stmt& statement) {
    if (const auto* decl = dynamic_cast<const VarDeclStmt*>(&statement)) {
        string mutability = decl->isConst ? "const " : "";
        string init = decl->initializer ? " = " + generateExpression(*decl->initializer) : "";
        return mutability + "auto " + decl->name.lexeme + init + ";";
    }
    if (const auto* assign = dynamic_cast<const AssignStmt*>(&statement)) {
        return assign->name.lexeme + " = " + generateExpression(*assign->value) + ";";
    }
    if (const auto* indexAssign = dynamic_cast<const IndexAssignStmt*>(&statement)) {
        return generateExpression(*indexAssign->array) + "[" + generateExpression(*indexAssign->index) + "] = " + generateExpression(*indexAssign->value) + ";";
    }
    if (const auto* show = dynamic_cast<const ShowStmt*>(&statement)) {
        stringstream ss;
        ss << "std::cout";
        for (size_t i = 0; i < show->values.size(); ++i) {
            ss << " << " << generateExpression(*show->values[i]);
            if (i + 1 < show->values.size()) ss << " << \" \"";
        }
        ss << " << std::endl;";
        return ss.str();
    }
    if (const auto* exprStmt = dynamic_cast<const ExprStmt*>(&statement)) {
        return generateExpression(*exprStmt->expression) + ";";
    }
    if (const auto* ret = dynamic_cast<const ReturnStmt*>(&statement)) {
        return "return " + (ret->value ? generateExpression(*ret->value) : "") + ";";
    }
    if (const auto* func = dynamic_cast<const FunctionDeclStmt*>(&statement)) {
        stringstream ss;
        ss << mapType(func->returnType.lexeme) << " " << func->name.lexeme << "(";
        for (size_t i = 0; i < func->parameters.size(); ++i) {
            ss << mapType(func->parameters[i].type.lexeme) << " " << func->parameters[i].name.lexeme;
            if (i + 1 < func->parameters.size()) ss << ", ";
        }
        ss << ") {\n";
        for (const auto& stmt : func->body) {
            ss << "    " << generateStatement(*stmt) << "\n";
        }
        ss << "}\n";
        return ss.str();
    }
    if (const auto* classDecl = dynamic_cast<const ClassDeclStmt*>(&statement)) {
        stringstream ss;
        ss << "struct " << classDecl->name.lexeme << " {\n";
        for (const auto& member : classDecl->members) {
            // changed by L - C++ structs cannot use 'auto' for member types. 
            // We use decltype() to magically figure out the type from the initializer value!
            if (const auto* varDecl = dynamic_cast<const VarDeclStmt*>(member.declaration.get())) {
                string initExpr = generateExpression(*varDecl->initializer);
                ss << "    decltype(" << initExpr << ") " << varDecl->name.lexeme << " = " << initExpr << ";\n";
            } else {
                ss << "    " << generateStatement(*member.declaration) << "\n";
            }
        }
        ss << "};\n";
        return ss.str();
    }
    if (const auto* when = dynamic_cast<const WhenStmt*>(&statement)) {
        stringstream ss;
        for (size_t i = 0; i < when->branches.size(); ++i) {
            if (i > 0) ss << " else ";
            ss << "if (" << generateExpression(*when->branches[i].condition) << ") {\n";
            for (const auto& stmt : when->branches[i].body) {
                ss << "    " << generateStatement(*stmt) << "\n";
            }
            ss << "}";
        }
        if (!when->defaultBody.empty()) {
            ss << " else {\n";
            for (const auto& stmt : when->defaultBody) {
                ss << "    " << generateStatement(*stmt) << "\n";
            }
            ss << "}\n";
        } else {
            ss << "\n";
        }
        return ss.str();
    }
    if (const auto* loop = dynamic_cast<const LoopStmt*>(&statement)) {
        stringstream ss;
        ss << "while (true) {\n";
        for (const auto& stmt : loop->body) {
            ss << "    " << generateStatement(*stmt) << "\n";
        }
        ss << "    if (" << generateExpression(*loop->condition) << ") { break; }\n";
        ss << "}\n";
        return ss.str();
    }
    if (dynamic_cast<const SkipStmt*>(&statement)) return "continue;";
    if (dynamic_cast<const StopStmt*>(&statement)) return "break;";
    
    if (const auto* capture = dynamic_cast<const CaptureStmt*>(&statement)) {
        return mapType(capture->type.lexeme) + " " + capture->name.lexeme + ";\n    std::cin >> " + capture->name.lexeme + ";";
    }
    throw runtime_error("Code generator: unsupported statement type");
}

string CodeGenerator::generateExpression(const Expr& expression) {
    if (const auto* lit = dynamic_cast<const LiteralExpr*>(&expression)) {
        // changed by L - Force text literals to strictly map to std::string for C++ compatbility
        if (lit->value.type == TokenType::TextLiteral) {
            return "std::string(" + lit->value.lexeme + ")";
        }
        return lit->value.lexeme;
    }
    if (const auto* var = dynamic_cast<const VariableExpr*>(&expression)) {
        if (var->name.lexeme == "self") return "this"; 
        return var->name.lexeme;
    }
    if (const auto* unary = dynamic_cast<const UnaryExpr*>(&expression)) {
        return unary->op.lexeme + generateExpression(*unary->right);
    }
    if (const auto* binary = dynamic_cast<const BinaryExpr*>(&expression)) {
        string op = binary->op.lexeme;
        return "(" + generateExpression(*binary->left) + " " + op + " " + generateExpression(*binary->right) + ")";
    }
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
    if (const auto* arrayExpr = dynamic_cast<const ArrayExpr*>(&expression)) {
        stringstream ss;
        ss << "std::vector<double>{"; 
        for (size_t i = 0; i < arrayExpr->elements.size(); ++i) {
            ss << generateExpression(*arrayExpr->elements[i]);
            if (i + 1 < arrayExpr->elements.size()) ss << ", ";
        }
        ss << "}";
        return ss.str();
    }
    if (const auto* indexExpr = dynamic_cast<const IndexExpr*>(&expression)) {
        return generateExpression(*indexExpr->array) + "[" + generateExpression(*indexExpr->index) + "]";
    }
    if (const auto* spawnExpr = dynamic_cast<const SpawnExpr*>(&expression)) {
        stringstream ss;
        ss << spawnExpr->className.lexeme << "{";
        for (size_t i = 0; i < spawnExpr->fields.size(); ++i) {
            ss << generateExpression(*spawnExpr->fields[i].value);
            if (i + 1 < spawnExpr->fields.size()) ss << ", ";
        }
        ss << "}";
        return ss.str();
    }
    if (const auto* methodCall = dynamic_cast<const MethodCallExpr*>(&expression)) {
        stringstream ss;
        string obj = generateExpression(*methodCall->object);
        if (obj == "this") obj += "->"; 
        else obj += ".";
        ss << obj << methodCall->method.lexeme << "(";
        for (size_t i = 0; i < methodCall->arguments.size(); ++i) {
            ss << generateExpression(*methodCall->arguments[i]);
            if (i + 1 < methodCall->arguments.size()) ss << ", ";
        }
        ss << ")";
        return ss.str();
    }
    if (const auto* memberExpr = dynamic_cast<const MemberExpr*>(&expression)) {
        string obj = generateExpression(*memberExpr->object);
        string memName = memberExpr->member.lexeme;
        if (memName == "length") memName = "size()"; 
        
        if (obj == "this") return obj + "->" + memName;
        return obj + "." + memName;
    }
    throw runtime_error("Code generator: unsupported expression type");
}