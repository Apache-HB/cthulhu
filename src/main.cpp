#include "lexer.cpp"

#include "ast.h"

#include <fstream>

int main(int argc, const char **argv) {
    (void)argc;
    (void)argv;
    using iflexer = ct::Lexer<std::ifstream>;
    iflexer lex(argv[1]);
}