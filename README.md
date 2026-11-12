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