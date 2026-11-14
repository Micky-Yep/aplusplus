# A++ Compiler

## Project Structure

`include/` contains the declarations for tokens, the lexer, the AST, the parser, semantic checking, and code generation.
`src/` contains the implementations of the compiler phases and the command-line driver.
`tests/` contains the lexer, parser, semantic checker, and code generator tests.
`samples/` contains A++ demo programs and test inputs.
`build/` contains local build outputs.

## Compiler Pipeline

The command-line driver in `src/main.cpp` runs the compiler in this order:

1. Tokenization and lexical analysis
2. Syntax analysis and AST construction
3. Semantic checking
4. C++ code generation

## Feature-to-File Map

| Feature or phase : Main files |
Token types and token data :`include/token.hpp` 
Tokenization and lexical analysis : `include/lexer.hpp`, `src/lexer.cpp` 
AST node definitions : `include/ast.hpp` 
Syntax analysis and AST construction : `include/parser.hpp`, `src/parser.cpp` 
Semantic checking : `include/semantic.hpp`, `src/semantic.cpp` 
C++ code generation : `include/codegen.hpp`, `src/codegen.cpp` 
Command-line driver and compiler pipeline : `src/main.cpp` 
Lexer tests : `tests/lexer_test.cpp` 
Parser tests : `tests/parser_test.cpp` 
Semantic checker tests : `tests/semantic_test.cpp` 
Code generator tests : `tests/codegen_test.cpp` 

## Parser tests

From the project root, run:

```sh
mkdir -p build
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude tests/parser_test.cpp src/lexer.cpp src/parser.cpp -o build/parser_test
./build/parser_test

```

## Lexer tests

Requires g++ with C++17 support.

From the project root, run in Linux or a WSL terminal:

```sh
mkdir -p build
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude tests/lexer_test.cpp src/lexer.cpp -o build/lexer_test
./build/lexer_test
```

Expected output:

```text
Lexer test passed.
```

The tests check token types and text, line and column positions,
keywords and identifiers, numbers, operators, strings, comments,
empty input, and invalid input.

## Lexer and tokens

The lexer reads A++ source code and returns a list of tokens.
Each token stores its type, original text, line and column.
Line and column numbers start at 1.

### Lexical rules

- Names start with a letter or underscore, followed by letters,
  digits or underscores.
- Names and keywords are case-sensitive.
- Numbers support integers and decimals, such as 12 and 72.5.
- A minus sign is a separate token.
- Text uses double quotes. Empty text is allowed.
- Text tokens keep their surrounding quotes.
- Text cannot contain a newline. Escape sequences are not supported.
- Spaces, tabs and line breaks are skipped.
- Comments start with // and continue to the end of the line.
- $capture is recognised as a special keyword.
- The lexer adds an EndOfFile token at the end.

### Errors

Invalid characters, incomplete text and invalid $capture keywords
cause an error. A single & or | also causes an error.

Error messages include the starting line and column.

Example:

Line 2, column 3: Unexpected character

### Repeated scanning

Calling scanTokens() resets the lexer before scanning.
The same lexer can scan its source again, including after an error.

### Scope

Recognising a keyword does not mean its feature is fully implemented.
The parser and semantic checker handle syntax and meaning.

## Build and Run the Compiler

From the project root, build the compiler:

```sh
mkdir -p build
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/main.cpp src/lexer.cpp src/parser.cpp src/semantic.cpp src/codegen.cpp -o build/aplus
