#ifndef RISCV_GEN_HPP
#define RISCV_GEN_HPP

#include <ostream>
#include <memory>
#include <cassert>
#include <string>

#include "KoopaIR.hpp"

class RiscVGenerator{

public:
void GenerateInstruction(const std::unique_ptr<koopa::Value>& ptr, std::ostream& os, const koopa::Program& program) const  {
    switch(ptr->type_){
        case koopa::ValueType::Return : {
            const auto ret = static_cast<koopa::Return*>(ptr.get());
            if (!ret->value_.has_value()) {
                os << "ret\n";
                return;
            }

            const auto val_id = *ret->value_;
            const auto& val_ptr = program.value_pool_[val_id];

            if (val_ptr->tmp_id_.has_value()) {
                os << "mv a0, " << "t" << std::to_string(*val_ptr->tmp_id_) << "\n";
            } else {
                assert(val_ptr->type_ == koopa::ValueType::Integer);
                const auto integer = static_cast<koopa::Integer*>(val_ptr.get());
                os << "li a0, " << integer->value_ << '\n';
            }
            os << "ret\n";
            break;
        }

        case koopa::ValueType::Integer : {
            const auto integer = static_cast<koopa::Integer*>(ptr.get());
            os << "li t0, " + std::to_string(integer->value_) + "\n";
            return;
        }

        case koopa::ValueType::Binary : {
            const auto binary = static_cast<koopa::Binary*>(ptr.get());

            assert(binary->tmp_id_.has_value());

            const auto& lhs = program.value_pool_[binary->lhs_];
            const auto& rhs = program.value_pool_[binary->rhs_];

            assert(lhs->type_ == koopa::ValueType::Integer);
            const auto lhs_integer = static_cast<koopa::Integer*>(lhs.get());
            assert(lhs_integer->value_ == 0);

            const auto dst = "t" + std::to_string(*binary->tmp_id_);

            if (binary->op_ == koopa::BinaryOP::EQ) {
                if (rhs->tmp_id_.has_value()) {
                    const auto src = "t" + std::to_string(*rhs->tmp_id_);

                    os << "seqz " << dst << ", " << src << '\n';
                } else {
                    assert(rhs->type_ == koopa::ValueType::Integer);
                    const auto integer = static_cast<koopa::Integer*>(rhs.get());

                    os << "li " << dst << ", " << integer->value_ << '\n';
                    os << "seqz " << dst << ", " << dst << '\n';
                }
                return;
            }

            if (binary->op_ == koopa::BinaryOP::SUB) {
                if (rhs->tmp_id_.has_value()) {
                    const auto src = "t" + std::to_string(*rhs->tmp_id_);

                    os << "sub " << dst << ", x0, " << src << '\n';
                } else {
                    assert(rhs->type_ == koopa::ValueType::Integer);
                    const auto integer = static_cast<koopa::Integer*>(rhs.get());
                    os << "li " << dst << ", " << integer->value_ << '\n';
                    os << "sub " << dst << ", x0, " << dst << '\n';
                }
                return;
            }
            assert(false);
        }
        default:{ return; }
    }
}

void GenerateRiscV(const koopa::Program& program, std::ostream& os) const {
    for (const auto& func : program.functions_) {
        os << ".text\n" << ".globl " << func->ident_ << '\n';
        os << func->ident_ << ":\n";
        for (const auto& block : func->basic_blocks_) {
            if (block->block_name_ != "entry") {
                os << block->block_name_ << ":\n";
            }

            for (const auto& val_id : block->instructions_) {
                const auto& ins = program.value_pool_[val_id];
                GenerateInstruction(ins, os, program);
            }
        }
    }
}

};

#endif // RISCV_GEN_HPP