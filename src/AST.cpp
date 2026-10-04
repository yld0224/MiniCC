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
void CompUnitNode::GenerateIR(std::ostream& os) const {
    func_def_->GenerateIR(os);
}

void FuncDefNode::PrintAST(size_t indent) const {
    std::string space(indent, ' ');
    std::cout << space << "FuncDef\n";
    std::cout << space << "  " << "FuncType: " << "INT\n";
    std::cout << space << "  " << "FuncName: " << ident_ << '\n';
    block_->PrintAST(indent + 2);
}
void FuncDefNode::GenerateIR(std::ostream& os) const {
    os << "fun " << "@" << ident_ << "(" << ")";
    if (type_ == DataType::INT) {
       os << ": " << "i32";
    }
    os << " {\n";
    os << "%" << "entry:\n";
    block_->GenerateIR(os);
    os << "}\n";
}

void FuncBlockNode::PrintAST(size_t indent) const {
    std::string space(indent, ' ');
    std::cout << space << "FuncBlock\n";
    for (size_t i = 0; i < statements_.size(); ++i) {
        statements_[i]->PrintAST(indent + 2);
    }
}
void FuncBlockNode::GenerateIR(std::ostream& os) const {
    for (size_t i = 0; i < statements_.size(); ++i) {
        statements_[i]->GenerateIR(os);
    }
}

void RetStmt::PrintAST(size_t indent) const {
    std::string space(indent, ' ');
    std::cout << space << "RetStmt\n";
    expr_->PrintAST(indent + 2); 
}
void RetStmt::GenerateIR(std::ostream& os) const {
    os << "ret" << ' ';
    expr_->GenerateIR(os);
    os << '\n';
}

void NumExpr::PrintAST(size_t indent) const {
    std::string space(indent, ' ');
    std::cout << space << "NumExpr: " << number_ << '\n'; 
}

void NumExpr::GenerateIR(std::ostream& os) const {
    os << number_;
}