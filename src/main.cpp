#include <cassert>
#include <cstdio>
#include <fstream>
#include <memory>

#include "AST.hpp"
#include "KoopaIR.hpp"
#include "RiscVGenerator.hpp"

extern FILE* yyin;
extern int yyparse(std::unique_ptr<CompUnitNode>& ast);


int main(int argc, char* argv[]) {
    assert(argc == 5);
    std::string flag = argv[1];
    const auto input = argv[2];
    const auto output = argv[4];

    yyin = fopen(input, "r");
    assert(yyin);

    std::unique_ptr<CompUnitNode> ast;
    const auto ret = yyparse(ast);
    assert(!ret);
    
    if (flag == "-ast") {
        ast->PrintAST(0);
        return 0;
    }
    

    koopa::Program program;
    koopa::IRBuilder builder(program);
    
    ast->GenerateIR(builder);

    std::ofstream ofs(output);
    if (flag == "-koopa") {
        program.PrintKoopa(ofs);
    } else if (flag == "-riscv") {
        RiscVGenerator gen;
        gen.GenerateRiscV(program, ofs);
    }
    ofs.close();
    
    return 0;
}