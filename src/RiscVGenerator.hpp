#ifndef RISCV_GEN_HPP
#define RISCV_GEN_HPP

#include <ostream>
#include <optional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "KoopaIR.hpp"

using koopa::Value, koopa::ValueID, koopa::Program;
using RegID = size_t;

class FunctionContext {

public:
    FunctionContext(const Program& program, std::ostream& os) : program_(program), os_(os) {}

    const Program& program_;
    std::ostream& os_;
    std::unordered_map<ValueID, int> stack_slots_;
    int frame_size_{0};
};

struct Register {
    bool allocatable_{false};
    std::optional<ValueID> value_{std::nullopt};
};

class RiscVGenerator{

public:
    RiscVGenerator() { registers_.resize(32); }

    void GenerateRiscV(Program& program, std::ostream& os);

private:
    void GenerateFunction(const koopa::Function& func);

    void GenerateBasicBlock(const koopa::BasicBlock& block);

    void GenerateInstruction(ValueID id);

    void GenerateRetIns(ValueID id);
    void GenerateBinaryIns(ValueID id);

    void LoadValue(ValueID val_id, RegID reg_id);
    void StoreValue(ValueID val_id, RegID reg_id);


    std::unique_ptr<FunctionContext> cur_functx_;
    std::vector<Register> registers_;
};

#endif // RISCV_GEN_HPP