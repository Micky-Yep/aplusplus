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

    // Check numbers and dots.
    Lexer numberLexer("0 12 72.5 -3 nums.length");
    vector<Token> numberTokens = numberLexer.scanTokens();

    vector<pair<TokenType, string>> expectedNumbers = {
        {TokenType::NumberLiteral, "0"},
        {TokenType::NumberLiteral, "12"},
        {TokenType::NumberLiteral, "72.5"},
        {TokenType::Minus, "-"},
        {TokenType::NumberLiteral, "3"},
        {TokenType::Identifier, "nums"},
        {TokenType::Dot, "."},
        {TokenType::Identifier, "length"},
        {TokenType::EndOfFile, ""}
    };

    if (numberTokens.size() != expectedNumbers.size()) {
        cerr << "Number test failed: wrong token count.\n";
        return 1;
    }

    for (size_t i = 0; i < expectedNumbers.size(); ++i) {
        if (numberTokens[i].type != expectedNumbers[i].first ||
            numberTokens[i].lexeme != expectedNumbers[i].second) {
            cerr << "Number test failed at token " << i << ".\n";
            return 1;
        }
    }
    
    // Check text and comments.
    Lexer textLexer("\"\" \"hello world\" \"// text\" // comment");
    vector<Token> textTokens = textLexer.scanTokens();

    vector<pair<TokenType, string>> expectedText = {
        {TokenType::TextLiteral, "\"\""},
        {TokenType::TextLiteral, "\"hello world\""},
        {TokenType::TextLiteral, "\"// text\""},
        {TokenType::EndOfFile, ""}
    };

    if (textTokens.size() != expectedText.size()) {
        cerr << "Text test failed: wrong token count.\n";
        return 1;
    }

    for (size_t i = 0; i < expectedText.size(); ++i) {
        if (textTokens[i].type != expectedText[i].first ||
            textTokens[i].lexeme != expectedText[i].second) {
            cerr << "Text test failed at token " << i << ".\n";
            return 1;
        }
    }

    // Check multiline text.
    if (!lexerThrows("\"hello\nworld\"")) {
        cerr << "Text test failed: multiline text should cause an error.\n";
        return 1;
    }

        // Check invalid capture.
    vector<string> invalidCaptures = {
        "$",
        "$captur",
        "$capture1",
        "$capture_name"
    };

    for (const string& source : invalidCaptures) {
        if (!lexerThrows(source)) {
            cerr << "Capture test failed for " << source << ".\n";
            return 1;
        }
    }

    // Check repeated scanning.
    Lexer repeatLexer("let value = 1;");
    vector<Token> firstTokens = repeatLexer.scanTokens();
    vector<Token> secondTokens = repeatLexer.scanTokens();

    if (firstTokens.size() != secondTokens.size()) {
        cerr << "Repeat test failed: different token counts.\n";
        return 1;
    }

    for (size_t i = 0; i < firstTokens.size(); ++i) {
        if (firstTokens[i].type != secondTokens[i].type ||
            firstTokens[i].lexeme != secondTokens[i].lexeme ||
            firstTokens[i].line != secondTokens[i].line ||
            firstTokens[i].column != secondTokens[i].column) {
            cerr << "Repeat test failed at token " << i << ".\n";
            return 1;
        }
    }

    // Check retry after an error.
    Lexer retryLexer("@");

    for (int attempt = 0; attempt < 2; ++attempt) {
        try {
            retryLexer.scanTokens();
            cerr << "Retry test failed: expected an error.\n";
            return 1;
        } catch (const runtime_error& error) {
            if (string(error.what()) !=
                "Line 1, column 1: Unexpected character") {
                cerr << "Retry test failed: wrong error.\n";
                return 1;
            }
        }
    }


    cout << "Lexer test passed.\n";
    return 0;
}