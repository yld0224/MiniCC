#include <cassert>
#include <cstdio>
#include <fstream>
#include <memory>

#include "AST.hpp"

extern FILE* yyin;
extern int yyparse(std::unique_ptr<CompUnitNode>& ast);


int main(int argc, char* argv[]) {
    assert(argc == 5);
    const auto input = argv[2];
    const auto output = argv[4];

    yyin = fopen(input, "r");
    assert(yyin);

    std::unique_ptr<CompUnitNode> ast;
    const auto ret = yyparse(ast);
    assert(!ret);

    std::ofstream ofs(output);
    ast->GenerateIR(ofs);
    return 0;
}