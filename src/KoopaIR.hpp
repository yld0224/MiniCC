#ifndef KOOPA_IR_HPP
#define KOOPA_IR_HPP

#include <cstddef>
#include <string>
#include <vector>
#include <optional>
#include <ostream>
#include <memory>

namespace koopa{

class Program;
class Function;
class BasicBlock;
class Value;

enum class Type{
    i32, unit
};

using ValueID = size_t;


class Program {

public:
    void PrintKoopa(std::ostream& os) const;

    std::vector<std::unique_ptr<Value>> value_pool_;
    std::vector<ValueID> global_values_;
    std::vector<std::unique_ptr<Function>> functions_;
};

class Function {

public:
    Type ret_type_{Type::unit};
    std::string ident_;
    std::vector<std::unique_ptr<BasicBlock>> basic_blocks_;
};

class BasicBlock {

public:
    explicit BasicBlock(std::string name) : block_name_(std::move(name)) {}

    std::string block_name_;
    std::vector<ValueID> instructions_;
};

enum class ValueType {
    Integer, ZeroInit, Undef, Aggregate, FuncArgRef, BlockArgRef,
    Alloc, GlobalAlloc, Load, Store, GetPtr,
    GetElemPtr, Binary, Branch, Jump, Call, Return
};



class Value {

public:
    explicit Value(ValueType type) : type_(type) {} 
    virtual ~Value() = default;
    virtual std::string PrintValue(const Program& program) const = 0;

    ValueType type_;
};

class Return : public Value {

public:
    explicit Return() : Value(ValueType::Return) {}
    explicit Return(ValueID val) : Value(ValueType::Return), value_(val) {}
    
    std::string PrintValue(const Program& program) const override;

    std::optional<ValueID> value_{std::nullopt};
};

class Integer : public Value {

public:
    explicit Integer(int val) : Value(ValueType::Integer), value_(val) {};

    std::string PrintValue(const Program& program) const override;

    int value_;
};


//    Integer(Integer),
//    ZeroInit(ZeroInit),
//    Undef(Undef),
//    Aggregate(Aggregate),
//    FuncArgRef(FuncArgRef),
//    BlockArgRef(BlockArgRef),
//    Alloc(Alloc),
//    GlobalAlloc(GlobalAlloc),
//    Load(Load),
//    Store(Store),
//    GetPtr(GetPtr),
//    GetElemPtr(GetElemPtr),
//    Binary(Binary),
//    Branch(Branch),
//    Jump(Jump),
//    Call(Call),
//    Return(Return),
//各类常量: 整数常量 (Integer), 零初始化器 (ZeroInit), 等等.
//参数引用: 函数参数引用 (FuncArgRef) 等, 用来指代传入的参数.
//内存分配: 全局内存分配 (GlobalAlloc, 所有的全局变量都是这个玩意) 和局部内存分配 (Alloc).
//访存指令: 加载 (Load) 和存储 (Store).
//指针运算: GetPtr 和 GetElemPtr.
//二元运算: Binary, 比如加减乘除模/比较之类的运算都属于此类.
//控制转移: 条件分支 (Branch) 和无条件跳转 (Jump).
//函数相关: 函数调用 (Call) 和函数返回 (Return).
}
#endif