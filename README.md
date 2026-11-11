## Parser tests

From the project root, run:

```sh
mkdir -p build
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude tests/parser_test.cpp src/lexer.cpp src/parser.cpp -o build/parser_test
./build/parser_test

```