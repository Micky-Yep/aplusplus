# Semantic checker demo

Valid: `when (true) { show 121; } default { show 0; }`
The checker accepts it. Expected interpreter output: `121`.

Invalid: `when (123) { show 121; } default { show 0; }`
The checker rejects it with `When condition must be Bool`.

From the repository root, run the parser and semantic checker test:

    mkdir -p build
    g++ -std=c++17 -Wall -Wextra -Iinclude tests/semantic_test.cpp src/parser.cpp src/semantic.cpp -o build/semantic_test
    ./build/semantic_test

The test reports two PASS lines. It does not execute the GOOFYS programs.
