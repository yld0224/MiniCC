#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

extern FILE* yyin;
extern int yyparse(std::unique_ptr<std::string>& ast);


int main(int argc, char* argv[]) {
    assert(argc == 5);
    const auto input = argv[2];

    yyin = fopen(input, "r");
    assert(yyin);

    std::unique_ptr<std::string> ast;
    const auto ret = yyparse(ast);
    assert(!ret);

    std::cout << *ast << std::endl;
    return 0;
}