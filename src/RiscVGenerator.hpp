#ifndef RISCV_GEN_HPP
#define RISCV_GEN_HPP

#include <cassert>
#include <ostream>
#include <optional>
#include <memory>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "KoopaIR.hpp"

using koopa::Value, koopa::ValueID, koopa::Program;
using RegID = size_t;
using BlockID = size_t;
using TmpValID = size_t;

class FunctionContext;

class ControlFlowGraph {

public:
    std::unordered_map<BlockID, std::unordered_set<BlockID>> graph_;
};

class InterferenceGraph {
public:
    void build(const koopa::Function& func, const Program& program);

    void simplify(FunctionContext& ctx);

    std::unordered_set<TmpValID>& neighbours(TmpValID tmp_id) {return graph_[tmp_id];}
private:
    std::unordered_map<TmpValID, std::unordered_set<TmpValID>> graph_;
    
};

class FunctionContext {

public:
    FunctionContext(const Program& program, std::ostream& os) : program_(program), os_(os) {}

    const Program& program_;
    std::ostream& os_;

    InterferenceGraph inter_graph_;
    std::stack<TmpValID> del_stack_;
    std::unordered_map<TmpValID, RegID> selected_regs_;
    
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
    bool SelectRegisters();

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