#include "parser.hpp"
#include "semantic.hpp"

#include <exception>
#include <iostream>
#include <string>
#include <vector>

bool checkWhen(Token condition, const char* label, const std::string& expectedError) {
    using T = TokenType;
    std::vector<Token> tokens{
        {T::When, "when", 1, 1}, {T::LeftParen, "(", 1, 6},
        condition, {T::RightParen, ")", 1, 11},
        {T::LeftBrace, "{", 1, 13}, {T::Show, "show", 1, 15},
        {T::NumberLiteral, "121", 1, 20}, {T::Semicolon, ";", 1, 23},
        {T::RightBrace, "}", 1, 25}, {T::Default, "default", 1, 27},
        {T::LeftBrace, "{", 1, 35}, {T::Show, "show", 1, 37},
        {T::NumberLiteral, "0", 1, 42}, {T::Semicolon, ";", 1, 43},
        {T::RightBrace, "}", 1, 45}, {T::EndOfFile, "", 2, 1}
    };

    try {
        auto program = Parser(tokens).parse();
        SemanticChecker checker;
        checker.checkProgram(program);
        if (!expectedError.empty()) {
            std::cerr << "FAIL: " << label << ": no error reported\n";
            return false;
        }
    } catch (const std::exception& e) {
        if (expectedError.empty() || expectedError != e.what()) {
            std::cerr << "FAIL: " << label << ": " << e.what() << '\n';
            return false;
        }
    }

    std::cout << "PASS: " << label << '\n';
    return true;
}

int main() {
    using T = TokenType;
    const bool valid = checkWhen({T::True, "true", 1, 7},
                                 "when (true) accepted", "");
    const bool invalid = checkWhen({T::NumberLiteral, "123", 1, 7},
                                   "when (123) rejected",
                                   "When condition must be Bool");
    return valid && invalid ? 0 : 1;
}
