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