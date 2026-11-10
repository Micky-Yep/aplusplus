#include "lexer.hpp"
#include "parser.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

bool parserThrows(const string& source) {
    try {
        Lexer lexer(source);
        Parser parser(lexer.scanTokens());
        parser.parse();
    } catch (const runtime_error&) {
        return true;
    }
    return false;
}

int main() {
    string source = "$capture first_name in text;\n let score = 72.5;\n let passed = score >= 50 && true;\n show\"Score:\", score, passed; when(score >= 80) {show score;} when(score>= 50) {show score;} default {show 0;} loop{show score;} until (score >= 100); loop{show score; skip; stop;} until (score >= 100);\n fn add(number: left, number: right) -> number {return left +right;}\n let result = add(3, 4);\n let nums = [1,2,3]; show nums[1], nums.length;\n";
    Lexer lexer(source);
    Parser parser(lexer.scanTokens());
    vector<StmtPtr> program = parser.parse();

    if (program.size() !=11)
    {
        cerr << "Parser test failed: expected 11 statements. \n";
        return 1;
    }

    if (!parserThrows("let missing = ;"))
    {
        cerr << "Parser test failed: malformed declaration should cause an error\n";
        return 1;
    }

    string arrayFunctionSource = "fn echo(number[]: values) -> number[] { return values; }";
    Lexer arrayFunctionLexer(arrayFunctionSource);
    Parser arrayFunctionParser(arrayFunctionLexer.scanTokens());
    vector<StmtPtr> arrayFunctionProgram = arrayFunctionParser.parse();
    if (arrayFunctionProgram.size() != 1 )
    {
        cerr << "Parser test failed: expected one array function \n";
        return 1;
    }
    
    auto* arrayFunction = dynamic_cast<FunctionDeclStmt*>(arrayFunctionProgram[0].get());
    if (arrayFunction == nullptr || arrayFunction->parameters.size() != 1 || !arrayFunction->parameters[0].isArray || !arrayFunction->returnsArray)
    {
        cerr << "Parser test failed: array function types were not recorded correctly\n";
        return 1;
    }
    
    string classSource = "setclass Student start public let age = 30; private const id = 1; public fn getAge() -> number { return self.age; } :end";
    Lexer classLexer(classSource);
    Parser classParser(classLexer.scanTokens());
    vector<StmtPtr> classProgram = classParser.parse();

    if (classProgram.size() != 1)
    {
        cerr << "Parser test failed: expected one class declaration\n";
        return 1;
    }

    auto* studentClass = dynamic_cast<ClassDeclStmt*>(classProgram[0].get());

    if (studentClass == nullptr || studentClass->members.size() != 3 || studentClass->members[0].access.type != TokenType::Public || studentClass->members[1].access.type != TokenType::Private || studentClass->members[2].access.type != TokenType::Public)
    {
        cerr << "Parser test failed: class members were not parsed correctly\n";
        return 1;
    }
    
    string spawnSource = "let student = spawn Student [age = 30, id = 1];";
    Lexer spawnLexer(spawnSource);
    Parser spawnParser(spawnLexer.scanTokens());
    vector<StmtPtr> spawnProgram = spawnParser.parse();
    if (spawnProgram.size() != 1)
    {
        cerr << "Parser test failed: expected one spawn declaration.\n";
        return 1;
    }
    
    auto* studentDecl = dynamic_cast<VarDeclStmt*>(spawnProgram[0].get());
    if (studentDecl == nullptr)
    {
        cerr << "Parser test failed: expected a variable declaration\n";
        return 1;
    }

    auto* spawnExpr = dynamic_cast<SpawnExpr*>(studentDecl->initializer.get());
    if (spawnExpr == nullptr || spawnExpr->className.lexeme != "Student" || spawnExpr->fields.size() != 2)
    {
        cerr << "Parser test failed: spawn expression was not parsed correctly\n";
        return 1;
    }
    
    string methodSource = "let result = student.getAge();";
    Lexer methodLexer(methodSource);
    Parser methodParser(methodLexer.scanTokens());
    vector<StmtPtr> methodProgram = methodParser.parse();

    if (methodProgram.size() != 1)
    {
        cerr << "Parser test failed: expected one method-call declaration\n";
        return 1;
    }
    
    auto* methodDecl = dynamic_cast<VarDeclStmt*>(methodProgram[0].get());
    auto* methodCall = methodDecl == nullptr ? nullptr : dynamic_cast<MethodCallExpr*>(methodDecl->initializer.get());

    if (methodProgram.size() != 1 || methodCall == nullptr || methodCall->method.lexeme != "getAge" || !methodCall->arguments.empty())
    {
        cerr << "Parser test failed: method call was not parsed correctly\n";
        return 1;
    }
    
    
    cout << "Parser test passed\n";
    return 0;
    
    
}