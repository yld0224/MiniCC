#include <cassert>
#include <cstdio>
#include <memory>

#include "AST.hpp"

extern FILE* yyin;
extern int yyparse(std::unique_ptr<CompUnitNode>& ast);


int main(int argc, char* argv[]) {
    assert(argc == 5);
    const auto input = argv[2];

    yyin = fopen(input, "r");
    assert(yyin);

    std::unique_ptr<CompUnitNode> ast;
    const auto ret = yyparse(ast);
    assert(!ret);

    ast->PrintAST(0);
    return 0;
}