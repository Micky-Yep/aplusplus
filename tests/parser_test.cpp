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
    
    cout << "Parser test passed\n";
    return 0;
    
    
}