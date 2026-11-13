#include "lexer.hpp"    // Brings in Phase 1: The Lexer (Text -> Tokens)
#include "parser.hpp"   // Brings in Phase 2: The Parser (Tokens -> Syntax Tree)
#include "semantic.hpp" // Brings in Phase 3: The Semantic Checker (Rule checking)
#include "codegen.hpp"  // Brings in Phase 4: The Code Generator (Tree -> C++ Code)

#include <iostream>     // Lets us print messages to the terminal screen
#include <string>       // Lets us use and manipulate text strings
#include <stdexcept>    // Lets us handle errors gracefully

using namespace std;

// -----------------------------------------------------------------------------
// HELPER FUNCTION 1: The Compiler Pipeline
// This function takes a raw string of A++ code, runs it through our entire
// 4-step assembly line, and hands back the final translated C++ code.
// -----------------------------------------------------------------------------
string generateTargetCode(const string& source) {
    // Phase 1: Read the raw text and break it into words/symbols (Tokens)
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    
    // Phase 2: Organize those tokens into a logical Abstract Syntax Tree (AST)
    Parser parser(tokens);
    auto ast = parser.parse();
    
    // Phase 3: Check the tree to make sure the logic is legal (variables exist, types match)
    SemanticChecker checker;
    checker.checkProgram(ast);
    
    // Phase 4: Translate the checked tree into standard C++ code
    CodeGenerator codegen;
    return codegen.generate(ast);
}

// -----------------------------------------------------------------------------
// HELPER FUNCTION 2: The "Find in Document" Checker
// This function acts like a robot using Ctrl+F. It looks at the C++ code we generated,
// and checks if the exact piece of text we *expect* to see is actually in there.
// If it's missing, it prints a helpful error message and stops the program.
// -----------------------------------------------------------------------------
void assertContains(const string& generated, const string& expected, const string& testName) {
    // string::npos means "no position found" (the text is missing from the document)
    if (generated.find(expected) == string::npos) {
        cerr << "\n[!] Test Failed: " << testName << "\n";
        cerr << "Expected to find this C++ code:\n  " << expected << "\n\n";
        cerr << "But our compiler generated this code instead:\n" << generated << "\n";
        exit(1); // Force the program to quit immediately with an error code (1)
    }
}

// -----------------------------------------------------------------------------
// MAIN FUNCTION: The Test Suite
// This is where we run mini A++ programs through our compiler and check the results.
// -----------------------------------------------------------------------------
int main() {
    cout << "Running Code Generator Tests...\n";

    // We wrap our tests in a "try" block. If the compiler crashes completely,
    // we can safely catch the error and print it instead of the program just dying.
    try {
        
        // TEST 1: Variables and Constants
        // We write a tiny A++ program with variables, and save the generated C++ code.
        string varCode = generateTargetCode("let score = 100; const max = 200; score = 150;");
        // Check if the C++ code contains the correct translation
        assertContains(varCode, "auto score = 100;", "Variable Declaration");
        assertContains(varCode, "const auto max = 200;", "Const Declaration");
        assertContains(varCode, "score = 150;", "Variable Assignment");

        // TEST 2: Print (Show) Statement
        string showCode = generateTargetCode("show \"Hello\", 123;");
        // A++ 'show' should become C++ 'std::cout'
        assertContains(showCode, "std::cout << std::string(\"Hello\") << \" \" << 123 << std::endl;", "Show Statement Translation");

        // TEST 3: Conditional If/Else (When/Default)
        string whenCode = generateTargetCode("let x = 1; when (x == 1) { show x; } default { show 0; }");
        // A++ 'when' should become C++ 'if'
        assertContains(whenCode, "if ((x == 1)) {", "When Branch Translation");
        // A++ 'default' should become C++ 'else'
        assertContains(whenCode, "} else {", "Default Branch Translation");

        // TEST 4: Loops and Loop Control (Skip/Stop)
        string loopCode = generateTargetCode("let c = 0; loop { c = c + 1; skip; } until(c > 5);");
        // A++ 'loop' should become C++ 'while (true)'
        assertContains(loopCode, "while (true) {", "Loop Start Generation");
        // A++ 'skip' should become C++ 'continue'
        assertContains(loopCode, "continue;", "Skip Statement Generation");
        // A++ 'until' should become a C++ 'if (...) { break; }'
        assertContains(loopCode, "if ((c > 5)) { break; }", "Until Condition Generation");

        // TEST 5: Functions and Math
        string funcCode = generateTargetCode("fn add(number: a, number: b) -> number { return a + b; }");
        // A++ 'number' parameters should become C++ 'double' parameters
        assertContains(funcCode, "double add(double a, double b) {", "Function Signature Translation");
        assertContains(funcCode, "return (a + b);", "Return Statement Translation");

        // TEST 6: Arrays (Lists of items)
        string arrayCode = generateTargetCode("let arr = [1, 2]; arr[0] = 5; show arr.length;");
        // A++ arrays should become C++ std::vector
        assertContains(arrayCode, "std::vector<double>{1, 2}", "Array Literal Translation");
        assertContains(arrayCode, "arr[0] = 5;", "Array Index Assignment");
        // A++ '.length' should become C++ '.size()'
        assertContains(arrayCode, "arr.size()", "Array Length Translation");

        // TEST 7: Classes and Object Spawning (OOP)
        // We make an A++ class called Player, and then spawn a new Player object.
        string classCode = generateTargetCode(
            "setclass Player start public let hp = 100; :end "
            "let p1 = spawn Player [hp = 50];"
        );
        // A++ 'setclass' should become a C++ 'struct'
        assertContains(classCode, "struct Player {", "Class Struct Generation");
        // A++ 'spawn' should instantiate the C++ struct
        assertContains(classCode, "Player{50}", "Spawn Object Instantiation");

        // If we made it all the way down here without crashing, every test passed!
        cout << "All Code Generator tests passed successfully!\n";

    } catch (const exception& e) {
        // This catches any unexpected errors (like the Lexer or Parser failing)
        cerr << "Test Suite Crashed with an exception: " << e.what() << "\n";
        return 1;
    }

    return 0; // Return 0 means the test program finished without issues
}