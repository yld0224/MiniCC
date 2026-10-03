#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "AST.hpp"


void CompUnitNode::PrintAST(size_t indent) const {
    std::string space(indent, ' ');
    std::cout << space << "CompUnit\n";
    func_def_->PrintAST(indent + 2);
}

void FuncDefNode::PrintAST(size_t indent) const {
    std::string space(indent, ' ');
    std::cout << space << "FuncDef\n";
    std::cout << space << "  " << "FuncType: " << "INT\n";
    std::cout << space << "  " << "FuncName: " << ident_ << '\n';
    block_->PrintAST(indent + 2);
}

void FuncBlockNode::PrintAST(size_t indent) const {
    std::string space(indent, ' ');
    std::cout << space << "FuncBlock\n";
    for (size_t i = 0; i < statements_.size(); ++i) {
        statements_[i]->PrintAST(indent + 2);
    }
}

void RetStmt::PrintAST(size_t indent) const {
    std::string space(indent, ' ');
    std::cout << space << "RetStmt\n";
    expr_->PrintAST(indent + 2); 
}

void NumExpr::PrintAST(size_t indent) const {
    std::string space(indent, ' ');
    std::cout << space << "NumExpr: " << number_ << '\n'; 
}
