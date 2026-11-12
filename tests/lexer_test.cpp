#include "lexer.hpp"

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <stdexcept>

using namespace std;

bool lexerThrows(const string& source) {
    try {
        Lexer lexer(source);
        lexer.scanTokens();
    } catch (const runtime_error&) {
        return true;
    }
    return false;
}

int main() {
    string source ="$capture first_name in text;\n // This comment should be skipped\n let score = 72.5;\n show score; \n let greeting = \"Hi\";\n when (score >=70 && true) {show greeting; }";

    Lexer lexer(source);
    vector<Token> tokens = lexer.scanTokens();

    vector<pair<TokenType, string>> expected = {
        {TokenType::Capture, "$capture"},
        {TokenType::Identifier, "first_name"},
        {TokenType::In, "in"},
        {TokenType::TextType, "text"},
        {TokenType::Semicolon, ";"},
        {TokenType::Let, "let"},
        {TokenType::Identifier, "score"},
        {TokenType::Assign, "="},
        {TokenType::NumberLiteral, "72.5"},
        {TokenType::Semicolon, ";"},
        {TokenType::Show, "show"},
        {TokenType::Identifier, "score"},
        {TokenType::Semicolon, ";"},
        {TokenType::Let, "let"},
        {TokenType::Identifier, "greeting"},
        {TokenType::Assign, "="},
        {TokenType::TextLiteral, "\"Hi\""},
        {TokenType::Semicolon, ";"},
        {TokenType::When, "when"},
        {TokenType::LeftParen, "("},
        {TokenType::Identifier, "score"},
        {TokenType::GreaterEqual, ">="},
        {TokenType::NumberLiteral, "70"},
        {TokenType::And, "&&"},
        {TokenType::True, "true"},
        {TokenType::RightParen, ")"},
        {TokenType::LeftBrace, "{"},
        {TokenType::Show, "show"},
        {TokenType::Identifier, "greeting"},
        {TokenType::Semicolon, ";"},
        {TokenType::RightBrace, "}"},
        {TokenType::EndOfFile, ""}
    };

    if (tokens.size() != expected.size()) {
        cerr << "Test failed: unexpected number of tokens.\n";
        return 1;
    }

    for (size_t i = 0; i < expected.size(); ++i) {
        if (tokens[i].type != expected[i].first || tokens[i].lexeme != expected[i].second) {
            cerr << "Test failed at token " << i << ".\n";
            return 1;
        }
    }
    
    // Check token positions.
    if (tokens[0].line != 1 || tokens[0].column != 1) {
        cerr << "Position test failed for $capture.\n";
        return 1;
    }

    if (tokens[5].line != 3 || tokens[5].column != 2) {
        cerr << "Position test failed for let.\n";
        return 1;
    }

    if (tokens[8].line != 3 || tokens[8].column != 14) {
        cerr << "Position test failed for 72.5.\n";
        return 1;
    }

    // Check empty input
    Lexer emptyLexer("");
    vector<Token> emptyTokens = emptyLexer.scanTokens();

    if (emptyTokens.size() != 1) {
        cerr << "Empty input test failed: expected one token.\n";
        return 1;
    }

    if (emptyTokens[0].type != TokenType::EndOfFile ||
        !emptyTokens[0].lexeme.empty() ||
        emptyTokens[0].line != 1 ||
        emptyTokens[0].column != 1) {
        cerr << "Empty input test failed: incorrect EOF token.\n";
        return 1;
    }

    string operatorSource = "+ - * / = == != < <= > >= && || ! ->";
    Lexer operatorLexer(operatorSource);
    vector<Token> operatorTokens = operatorLexer.scanTokens();

    vector<pair<TokenType, string>> expectedOperators = {
        {TokenType::Plus, "+"},
        {TokenType::Minus, "-"},
        {TokenType::Multiply, "*"},
        {TokenType::Divide, "/"},
        {TokenType::Assign, "="},
        {TokenType::Equal, "=="},
        {TokenType::NotEqual, "!="},
        {TokenType::Less, "<"},
        {TokenType::LessEqual, "<="},
        {TokenType::Greater, ">"},
        {TokenType::GreaterEqual, ">="},
        {TokenType::And, "&&"},
        {TokenType::Or, "||"},
        {TokenType::Not, "!"},
        {TokenType::Arrow, "->"},
        {TokenType::EndOfFile, ""}
    };

    if (operatorTokens.size() != expectedOperators.size()) {
        cerr << "Test failed: unexpected number of operator tokens.\n";
        return 1;
    }

    for (size_t i = 0; i < expectedOperators.size(); ++i) {
        if (operatorTokens[i].type != expectedOperators[i].first || operatorTokens[i].lexeme != expectedOperators[i].second) {
            cerr << "Operator test failed at token " << i << ".\n";
            return 1;
        }
    }

    if (!lexerThrows("\"unfinished"))
    {
        cerr << "Test failed: unterminated text should cause an error.\n";
        return 1;
    }
    if (!lexerThrows("@"))
    {
        cerr << "Test failed: unexpected character should cause an error.\n";
        return 1;
    }
    if (!lexerThrows("&"))
    {
        cerr << "Test failed: a single & should cause an error.\n";
        return 1;
    }
    if (!lexerThrows("$captureName"))
    {
        cerr << "Test failed: $captureName should not be the $capture keyword.\n";
        return 1;
    }
    
    // Check error position.
    try {
        Lexer invalidLexer("\n  @");
        invalidLexer.scanTokens();
        cerr << "Test failed: expected an error.\n";
        return 1;
    } catch (const runtime_error& error) {
        string message = error.what();

        if (message != "Line 2, column 3: Unexpected character") {
            cerr << "Wrong error message: " << message << "\n";
            return 1;
        }
    }

    // Check keywords and names.
    Lexer nameLexer("let letter const constant true trueValue score Score _name name2");
    vector<Token> nameTokens = nameLexer.scanTokens();

    vector<pair<TokenType, string>> expectedNames = {
        {TokenType::Let, "let"},
        {TokenType::Identifier, "letter"},
        {TokenType::Const, "const"},
        {TokenType::Identifier, "constant"},
        {TokenType::True, "true"},
        {TokenType::Identifier, "trueValue"},
        {TokenType::Identifier, "score"},
        {TokenType::Identifier, "Score"},
        {TokenType::Identifier, "_name"},
        {TokenType::Identifier, "name2"},
        {TokenType::EndOfFile, ""}
    };

    if (nameTokens.size() != expectedNames.size()) {
        cerr << "Name test failed: wrong token count.\n";
        return 1;
    }

    for (size_t i = 0; i < expectedNames.size(); ++i) {
        if (nameTokens[i].type != expectedNames[i].first ||
            nameTokens[i].lexeme != expectedNames[i].second) {
            cerr << "Name test failed at token " << i << ".\n";
            return 1;
        }
    }
    
    cout << "Lexer test passed.\n";
    return 0;
}