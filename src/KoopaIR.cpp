#include <ostream>
#include <cassert>

#include "KoopaIR.hpp"

namespace koopa {

void Program::PrintKoopa(std::ostream& os) const {
    for (size_t i = 0; i < functions_.size(); ++i){
        const auto& fun = functions_[i];
        os << "fun " << "@" << fun->ident_ << "()";
        if (fun->ret_type_ == Type::i32) {
            os << ": i32 {\n";
        } else {
            os << " {\n";
        }
        for (const auto& block : fun->basic_blocks_) {
            os << "%" << block->block_name_ << ":\n";
            for (const auto& val_id : block->instructions_) {
                const auto& ins = this->value_pool_[val_id];
                os << "  " << ins->PrintValue(*this) << '\n';
            }
        }
        os << "}\n";
    }
} 

std::string Return::PrintValue(const Program& program) const {
    if (value_.has_value()) {
        const auto& ins = program.value_pool_[*value_];
        if (ins->tmp_id_.has_value()) {
            return std::string("ret %") + std::to_string(*(ins->tmp_id_));
        }
        assert(ins->type_ == ValueType::Integer);

        return "ret " + ins->PrintValue(program);
    }
    return "ret";
}

std::string Integer::PrintValue([[maybe_unused]] const Program& program) const {
    return std::to_string(value_);
}

std::string Binary::PrintValue(const Program& program) const {
    assert(tmp_id_.has_value());

    const auto& l_val = program.value_pool_[lhs_];
    const auto& r_val = program.value_pool_[rhs_];

    std::string l;
    std::string r;
    if (l_val->tmp_id_.has_value()) {
        l = "%" + std::to_string(*(l_val->tmp_id_));
    } else {
        assert(l_val->type_ == ValueType::Integer);
        l = l_val->PrintValue(program);
    }

    if (r_val->tmp_id_.has_value()) {
        r = "%" + std::to_string(*(r_val->tmp_id_));
    } else {
        assert(r_val->type_ == ValueType::Integer);
        r = r_val->PrintValue(program);
    }

    switch(this->op_) {
        case (BinaryOP::SUB) : {
            return "%" + std::to_string(*tmp_id_) + " = " + "sub " + l + ", " + r;
        }
        case (BinaryOP::EQ) : {
            return "%" + std::to_string(*tmp_id_) + " = " + "eq " + l + ", " + r;
        }
        default: {
            return "invalid";
        }
    }
}

}