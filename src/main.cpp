#include <iostream>
#include <fstream>      // Allows us to open and read files from the computer
#include <sstream>      // Helps us buffer the file's text into one big string
#include "lexer.hpp"
#include "parser.hpp"
#include "semantic.hpp"
#include "codegen.hpp"

using namespace std;

// 'argc' counts how many words the user typed in the terminal.
// 'argv' is an array holding those exact words.
// Example: If you type `./aplus code.aplus`, argc is 2. 
// argv[0] is "./aplus" and argv[1] is "code.aplus".
int main(int argc, char* argv[]) {
    
    // Check if the user actually gave us a file to compile
    if (argc < 2) {
        cerr << "Usage: aplus <source-file>\n"; // Tell them how to use the program
        return 1; // Return 1 to signal that the program closed with an error
    }

    // Try to open the file they asked for
    ifstream file(argv[1]); // argv[1] holds the file name they typed
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << argv[1] << "\n";
        return 1; 
    }

    // Read the entire file into a single text string
    stringstream buffer;
    buffer << file.rdbuf(); // Pour all the file's contents into our buffer
    string source = buffer.str(); // Convert it into a standard C++ string

    // The Compiler Assembly Line
    // We put this in a 'try' block. If any step throws an error (like a missing semicolon),
    // it safely skips the rest and jumps down to the 'catch' block to print the error.
    try {
        
        // --- PHASE 1: Lexer ---
        // Take the raw text string and chop it up into meaningful 'Tokens' (words, numbers, symbols)
        Lexer lexer(source);
        auto tokens = lexer.scanTokens();

        // --- PHASE 2: Parser ---
        // Take that list of tokens and organize them into an Abstract Syntax Tree (AST).
        // This makes sure the grammar is correct .
        Parser parser(tokens);
        auto ast = parser.parse();

        // --- PHASE 3: Semantic Checker ---
        // Look at the AST and make sure the logic makes sense.
        // Are we using variables before declaring them or are types matching up
        SemanticChecker checker;
        checker.checkProgram(ast);

        // --- PHASE 4: Code Generator ---
        // Take our perfectly valid AST and translate it into standard C++ code.
        CodeGenerator codegen;
        string cppCode = codegen.generate(ast);

        // Output the final result!
        // We print the generated C++ code to the terminal so we can see it or save it.
        cout << "// --- Generated C++ Code ---\n";
        cout << cppCode;

    } catch (const exception& e) {
        // If the Lexer, Parser, or Checker threw an error, it gets caught here.
        cerr << "Compilation Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}