#include <ostream>
#include <memory>
#include <cassert>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "KoopaIR.hpp"
#include "RiscVGenerator.hpp"

using koopa::ValueID, koopa::Program;
using RegID = size_t;

static const std::unordered_map<RegID, std::string> REGNAME = {
{0,  "zero"}, {1,  "ra"},  {2,  "sp"},  {3,  "gp"},  {4,  "tp"},
{5,  "t0"},   {6,  "t1"},  {7,  "t2"},  {8,  "s0"},  {9,  "s1"},
{10, "a0"},   {11, "a1"},  {12, "a2"},  {13, "a3"},  {14, "a4"},
{15, "a5"},   {16, "a6"},  {17, "a7"},  {18, "s2"},  {19, "s3"},
{20, "s4"},   {21, "s5"},  {22, "s6"},  {23, "s7"},  {24, "s8"},
{25, "s9"},   {26, "s10"}, {27, "s11"}, {28, "t3"},  {29, "t4"},
{30, "t5"},   {31, "t6"}
};

static const std::unordered_map<RegID, std::string> TMPREGNAME = {
    {0, "t0"}, {1, "t1"}, {2, "t2"}, {3, "t3"}, {4, "t4"}, {5, "t5"}, {6, "t6"}
};

constexpr size_t TMPREGSIZE = 7;

void InterferenceGraph::build(const koopa::Function& func, const Program& program){
        assert(func.basic_blocks_.size() == 1);
        const auto& block = func.basic_blocks_[0];

        const size_t ins_n = block->instructions_.size();
        std::vector<std::unordered_set<TmpValID>> in_sets(ins_n);
        std::vector<std::unordered_set<TmpValID>> out_sets(ins_n);
        
        for (int j = ins_n - 1; j >= 0; --j) {
            if (j < ins_n - 1) {
                out_sets[j] = in_sets[j + 1];
            }
           
            const auto val_id = block->instructions_[j];
            const auto& ins = program.value_pool_[val_id];
            std::unordered_set<TmpValID> def{};
            std::unordered_set<TmpValID> use{};

            switch(ins->type_) {
                case (koopa::ValueType::Return) : {
                    const auto ret_ptr = static_cast<koopa::Return*>(ins.get());
                    if (ret_ptr->value_.has_value()) {
                        const auto& ret_child = program.value_pool_[*ret_ptr->value_];
                        if (ret_child->tmp_id_.has_value()) {
                            use.insert(*ret_child->tmp_id_);
                        }
                    }
                    break;
                }

                case (koopa::ValueType::Binary) : {
                    const auto bin_ptr = static_cast<koopa::Binary*>(ins.get());
                    const auto& l_child = program.value_pool_[bin_ptr->lhs_];
                    const auto& r_child = program.value_pool_[bin_ptr->rhs_];
                    
                    if (l_child->tmp_id_.has_value()) {
                        use.insert(*l_child->tmp_id_);
                    }
                    if (r_child->tmp_id_.has_value()) {
                        use.insert(*r_child->tmp_id_);
                    }
                    break;
                }                
                
                default : {}
            }

            if (ins->tmp_id_.has_value()) {def.insert(*ins->tmp_id_);}
            std::unordered_set<TmpValID> out = out_sets[j];
            
            for (const auto tmp_id : def) { out.erase(tmp_id); }
            for (const auto tmp_id : use) { out.insert(tmp_id); }
            in_sets[j] = out;
        }
        
        for (size_t i = 0; i < ins_n; ++i) {
            const auto val_id = block->instructions_[i];
            const auto& ins = program.value_pool_[val_id];

            if (!ins->tmp_id_.has_value()) continue;

            const TmpValID def = *ins->tmp_id_;
            graph_.try_emplace(def);

            for (const auto live : out_sets[i]) {
                if (live == def) continue;

                graph_[def].insert(live);
                graph_[live].insert(def);
            }
        }

}

void InterferenceGraph::simplify(FunctionContext& ctx) {
    auto graph = graph_;
    while (!graph.empty()) {
        bool suc = false;
        for (const auto& [tmp_id, set] : graph) {
            if (set.size() < TMPREGSIZE) {
                for (auto& edge_to : set) {
                    graph[edge_to].erase(tmp_id);
                }
                graph.erase(tmp_id);
                ctx.del_stack_.push(tmp_id);
                suc = true;
                break;
            }
        }
        if (suc) {continue;}
        auto iter = graph.begin();
        auto& set = graph[iter->first];
        for (auto& edge_to : set) {
            graph[edge_to].erase(iter->first);
        }
        ctx.del_stack_.push(iter->first);
        graph.erase(iter);
    }
}

bool RiscVGenerator::SelectRegisters() {
    cur_functx_->selected_regs_.clear();

    while (!cur_functx_->del_stack_.empty()){
        auto tmp_id = cur_functx_->del_stack_.top();
        cur_functx_->del_stack_.pop();

        bool suc;
        for (size_t i = 0; i < TMPREGSIZE; ++i) {
            suc = true;
            for (const auto neighbour : cur_functx_->inter_graph_.neighbours(tmp_id)){
                auto iter = cur_functx_->selected_regs_.find(neighbour);
                if (iter != cur_functx_->selected_regs_.end() && 
                    iter->second == i) {
                    suc = false;
                    break;
                }
            }
            if (suc) { 
                cur_functx_->selected_regs_[tmp_id] = i;
                break;
            }
        }
        if (!suc) {return false;}
    }

    return true;
}

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

    cur_functx_->inter_graph_.build(func, cur_functx_->program_);
    cur_functx_->inter_graph_.simplify(*cur_functx_);
    bool suc = SelectRegisters();

    assert(suc);

    for (const auto& block : func.basic_blocks_) {
        this->cur_functx_->os_ << block->block_name_ << ":\n";
        
        GenerateBasicBlock(*block);
    }
}

void RiscVGenerator::GenerateBasicBlock(const koopa::BasicBlock& block) {
    for (const auto& val_id : block.instructions_) {
        GenerateInstruction(val_id);
    }
}

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