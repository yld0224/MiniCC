#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "AST.hpp"
#include "KoopaIR.hpp"


void CompUnitNode::PrintAST(size_t indent) const {
    std::string space(indent, ' ');
    std::cout << space << "CompUnit\n";
    func_def_->PrintAST(indent + 2);
}
void CompUnitNode::GenerateIR(koopa::IRBuilder& builder) const {
    func_def_->GenerateIR(builder);
}

void FuncDefNode::PrintAST(size_t indent) const {
    std::string space(indent, ' ');
    std::cout << space << "FuncDef\n";
    std::cout << space << "  " << "FuncType: " << "INT\n";
    std::cout << space << "  " << "FuncName: " << ident_ << '\n';
    block_->PrintAST(indent + 2);
}
void FuncDefNode::GenerateIR(koopa::IRBuilder& builder) const {
    koopa::Function* func = new koopa::Function();

    func->ident_ = this->ident_;
    if (this->type_ == DataType::INT){
        func->ret_type_ = koopa::Type::i32;
    }
    builder.program_.functions_.push_back(std::unique_ptr<koopa::Function>(func));
    builder.current_function_ = func;
    
    koopa::BasicBlock* entry = new koopa::BasicBlock("entry");
    func->basic_blocks_.push_back(std::unique_ptr<koopa::BasicBlock>(entry));
    builder.current_block_ = entry;

    block_->GenerateIR(builder);
}

void FuncBlockNode::PrintAST(size_t indent) const {
    std::string space(indent, ' ');
    std::cout << space << "FuncBlock\n";
    for (size_t i = 0; i < statements_.size(); ++i) {
        statements_[i]->PrintAST(indent + 2);
    }
}
void FuncBlockNode::GenerateIR(koopa::IRBuilder& builder) const {
    for (size_t i = 0; i < statements_.size(); ++i) {
        statements_[i]->GenerateIR(builder);
    }
}

void RetStmt::PrintAST(size_t indent) const {
    std::string space(indent, ' ');
    std::cout << space << "RetStmt\n";
    expr_->PrintAST(indent + 2); 
}
void RetStmt::GenerateIR(koopa::IRBuilder& builder) const {
    koopa::ValueID val = this->expr_->GenerateIR(builder);
    
    koopa::ValueID nxtid = builder.program_.value_pool_.size();
    builder.program_.value_pool_.push_back(std::make_unique<koopa::Return>(val));
    
    builder.current_block_->instructions_.push_back(nxtid);
}

void NumExpr::PrintAST(size_t indent) const {
    std::string space(indent, ' ');
    std::cout << space << "NumExpr: " << number_ << '\n'; 
}

koopa::ValueID NumExpr::GenerateIR(koopa::IRBuilder& builder) const {
    koopa::ValueID nxtid = builder.program_.value_pool_.size();
    builder.program_.value_pool_.push_back(std::make_unique<koopa::Integer>(number_));
    return nxtid;
}

void UnaryExpr::PrintAST(size_t indent) const {
    std::string space(indent, ' ');
    std::cout << space << "UnaryExpr\n";
    std::cout << space << "  " << "OP: ";
    switch(op_){
        case (UnaryOP::NEG) : { std::cout << "NEG\n"; break; } 
        case (UnaryOP::POS) : { std::cout << "POS\n"; break; }
        case (UnaryOP::NOT) : { std::cout << "NOT\n"; break; }
    }
    operand_->PrintAST(indent + 2);
}

koopa::ValueID UnaryExpr::GenerateIR(koopa::IRBuilder& builder) const {
    if (this->op_ == UnaryOP::POS) {
        koopa::ValueID nxtid = this->operand_->GenerateIR(builder);
        
        return nxtid;
    } else if (op_ == UnaryOP::NEG) {
        koopa::ValueID rhs_id = this->operand_->GenerateIR(builder);

        koopa::ValueID lhs_id = builder.program_.value_pool_.size();
        builder.program_.value_pool_.push_back(std::make_unique<koopa::Integer>(0));
        
        koopa::ValueID nxtid = builder.program_.value_pool_.size();
        
        auto ptr = std::make_unique<koopa::Binary>(koopa::BinaryOP::SUB, lhs_id, rhs_id);
        ptr->tmp_id_ = builder.current_function_->tmp_value_id_;
        builder.current_function_->tmp_value_id_++;

        builder.program_.value_pool_.push_back(std::move(ptr));
        builder.current_block_->instructions_.push_back(nxtid);

        return nxtid;
    } else if (op_ == UnaryOP::NOT) {
        koopa::ValueID rhs_id = operand_->GenerateIR(builder);

        koopa::ValueID lhs_id = builder.program_.value_pool_.size();
        builder.program_.value_pool_.push_back( std::make_unique<koopa::Integer>(0));

        koopa::ValueID nxtid = builder.program_.value_pool_.size();

        auto ptr = std::make_unique<koopa::Binary>(koopa::BinaryOP::EQ, lhs_id, rhs_id);
        ptr->tmp_id_ = builder.current_function_->tmp_value_id_;
        builder.current_function_->tmp_value_id_++;

        builder.program_.value_pool_.push_back(std::move(ptr));
        builder.current_block_->instructions_.push_back(nxtid);

        return nxtid;
    }

    return -1;
}

void BinaryExpr::PrintAST(size_t indent) const {
    std::string space(indent, ' ');
    std::cout << space << "BinaryExpr\n";
    std::cout << space << "  " << "OP: ";
    switch(op_){
        case (BinaryOP::ADD) : { std::cout << "ADD\n"; break; } 
        case (BinaryOP::SUB) : { std::cout << "SUB\n"; break; } 
        case (BinaryOP::MUL) : { std::cout << "MUL\n"; break; } 
        case (BinaryOP::DIV) : { std::cout << "DIV\n"; break; } 
        case (BinaryOP::MOD) : { std::cout << "MOD\n"; break; } 
        case (BinaryOP::LS) : { std::cout << "LS\n"; break; } 
        case (BinaryOP::LE) : { std::cout << "LE\n"; break; } 
        case (BinaryOP::EQ) : { std::cout << "EQ\n"; break; } 
        case (BinaryOP::NE) : { std::cout << "NE\n"; break; } 
        case (BinaryOP::GE) : { std::cout << "GE\n"; break; } 
        case (BinaryOP::GT) : { std::cout << "GT\n"; break; } 
        case (BinaryOP::AND) : { std::cout << "AND\n"; break; } 
        case (BinaryOP::OR) : { std::cout << "OR\n"; break; }
    }
     
    lhs_->PrintAST(indent + 2);
    rhs_->PrintAST(indent + 2);
}

koopa::ValueID BinaryExpr::GenerateIR(koopa::IRBuilder& builder) const {
    koopa::ValueID lhs_id = this->lhs_->GenerateIR(builder);
    koopa::ValueID rhs_id = this->rhs_->GenerateIR(builder);

    koopa::ValueID nxt_id = builder.program_.value_pool_.size();

    std::unique_ptr<koopa::Binary> ptr;
    switch(op_){
        case (BinaryOP::ADD) : { ptr = std::make_unique<koopa::Binary>(koopa::BinaryOP::ADD, lhs_id, rhs_id); break; } 
        case (BinaryOP::SUB) : { ptr = std::make_unique<koopa::Binary>(koopa::BinaryOP::SUB, lhs_id, rhs_id); break; } 
        case (BinaryOP::MUL) : { ptr = std::make_unique<koopa::Binary>(koopa::BinaryOP::MUL, lhs_id, rhs_id); break; } 
        case (BinaryOP::DIV) : { ptr = std::make_unique<koopa::Binary>(koopa::BinaryOP::DIV, lhs_id, rhs_id); break; } 
        case (BinaryOP::MOD) : { ptr = std::make_unique<koopa::Binary>(koopa::BinaryOP::MOD, lhs_id, rhs_id); break; } 
        case (BinaryOP::LS)  : { ptr = std::make_unique<koopa::Binary>(koopa::BinaryOP::LT, lhs_id, rhs_id);  break; } 
        case (BinaryOP::LE)  : { ptr = std::make_unique<koopa::Binary>(koopa::BinaryOP::LE, lhs_id, rhs_id);  break; } 
        case (BinaryOP::EQ)  : { ptr = std::make_unique<koopa::Binary>(koopa::BinaryOP::EQ, lhs_id, rhs_id);  break; } 
        case (BinaryOP::NE)  : { ptr = std::make_unique<koopa::Binary>(koopa::BinaryOP::NE, lhs_id, rhs_id);  break; } 
        case (BinaryOP::GE)  : { ptr = std::make_unique<koopa::Binary>(koopa::BinaryOP::GE, lhs_id, rhs_id);  break; } 
        case (BinaryOP::GT)  : { ptr = std::make_unique<koopa::Binary>(koopa::BinaryOP::GT, lhs_id, rhs_id);  break; } 
        case (BinaryOP::AND) : { ptr = std::make_unique<koopa::Binary>(koopa::BinaryOP::AND, lhs_id, rhs_id); break; } 
        case (BinaryOP::OR)  : { ptr = std::make_unique<koopa::Binary>(koopa::BinaryOP::OR, lhs_id, rhs_id);  break; }
    }

    ptr->tmp_id_ = builder.current_function_->tmp_value_id_;
    builder.current_function_->tmp_value_id_++;

    builder.program_.value_pool_.push_back(std::move(ptr));
    builder.current_block_->instructions_.push_back(nxt_id);

    return nxt_id;
}