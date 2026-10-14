#include <ostream>
#include <memory>
#include <cassert>
#include <string>

#include "KoopaIR.hpp"
#include "RiscVGenerator.hpp"

using koopa::ValueID, koopa::Program;


void RiscVGenerator::GenerateInstruction(ValueID id) {
    const auto& ptr = cur_functx_->program_.value_pool_[id];

    switch(ptr->type_){
        case koopa::ValueType::Return : { GenerateRetIns(id); return; }
        case koopa::ValueType::Binary : { GenerateBinaryIns(id); return; }
        default:{ return; }
    }
}

void RiscVGenerator::GenerateRiscV(Program& program, std::ostream& os) {
    for (const auto& func : program.functions_) {
        os << ".text\n" << ".globl " << func->ident_ << '\n';
        os << func->ident_ << ":\n";
        
        this->cur_functx_ = std::make_unique<FunctionContext>(program, os);
        
        this->GenerateFunction(*func);
    }
}

void RiscVGenerator::GenerateFunction(const koopa::Function& func){
    for (const auto& block : func.basic_blocks_) {
        this->cur_functx_->os_ << "entry:\n";

        // AnalyzeFunction(func);
        
        GenerateBasicBlock(*block);
    }
}

void RiscVGenerator::GenerateBasicBlock(const koopa::BasicBlock& block) {
    for (const auto& val_id : block.instructions_) {
        GenerateInstruction(val_id);
    }
}

using RegID = size_t;
void RiscVGenerator::LoadValue(ValueID val_id, RegID reg_id) {}
void RiscVGenerator::StoreValue(ValueID val_id, RegID reg_id) {}

void RiscVGenerator::GenerateRetIns(ValueID id) {
        //    const auto val_id = *ret->value_;
        //    const auto& val_ptr = program.value_pool_[val_id];

        //    if (val_ptr->tmp_id_.has_value()) {
        //        os << "mv a0, " << "t" << std::to_string(*val_ptr->tmp_id_) << "\n";
        //    } else {
        //        assert(val_ptr->type_ == koopa::ValueType::Integer);
        //        const auto integer = static_cast<koopa::Integer*>(val_ptr.get());
        //        os << "li a0, " << integer->value_ << '\n';
        //    }
        //    os << "ret\n";
        //    break;
        //}
}

void RiscVGenerator::GenerateBinaryIns(ValueID id) {
        //    const auto binary = static_cast<koopa::Binary*>(ptr.get());

        //    assert(binary->tmp_id_.has_value());

        //    const auto& lhs = program.value_pool_[binary->lhs_];
        //    const auto& rhs = program.value_pool_[binary->rhs_];

        //    assert(lhs->type_ == koopa::ValueType::Integer);
        //    const auto lhs_integer = static_cast<koopa::Integer*>(lhs.get());
        //    assert(lhs_integer->value_ == 0);

        //    const auto dst = "t" + std::to_string(*binary->tmp_id_);

        //    if (binary->op_ == koopa::BinaryOP::EQ) {
        //        if (rhs->tmp_id_.has_value()) {
        //            const auto src = "t" + std::to_string(*rhs->tmp_id_);

        //            os << "seqz " << dst << ", " << src << '\n';
        //        } else {
        //            assert(rhs->type_ == koopa::ValueType::Integer);
        //            const auto integer = static_cast<koopa::Integer*>(rhs.get());

        //            os << "li " << dst << ", " << integer->value_ << '\n';
        //            os << "seqz " << dst << ", " << dst << '\n';
        //        }
        //        return;
        //    }

        //    if (binary->op_ == koopa::BinaryOP::SUB) {
        //        if (rhs->tmp_id_.has_value()) {
        //            const auto src = "t" + std::to_string(*rhs->tmp_id_);

        //            os << "sub " << dst << ", x0, " << src << '\n';
        //        } else {
        //            assert(rhs->type_ == koopa::ValueType::Integer);
        //            const auto integer = static_cast<koopa::Integer*>(rhs.get());
        //            os << "li " << dst << ", " << integer->value_ << '\n';
        //            os << "sub " << dst << ", x0, " << dst << '\n';
        //        }
        //        return;
        //    }
        //    assert(false);

}