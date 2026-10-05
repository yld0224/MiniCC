#include <ostream>

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
        return "ret " + ins->PrintValue(program);
    }
    return "ret";
}

std::string Integer::PrintValue(const Program& program) const {
    return std::to_string(value_);
}
}