#ifndef RISCV_GEN_HPP
#define RISCV_GEN_HPP

#include <ostream>
#include <memory>

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
            GenerateInstruction(val_ptr, os,  program);

            os << "mv a0, t0\n";
            os << "ret\n";
            break;
        }

        case koopa::ValueType::Integer : {
            const auto integer = static_cast<koopa::Integer*>(ptr.get());
            os << "li t0, " + std::to_string(integer->value_) + "\n";
            return;
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