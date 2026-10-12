#ifndef AST_HPP
#define AST_HPP

#include <memory>
#include <string>
#include <vector>

#include "KoopaIR.hpp"

class BaseNode;
class CompUnitNode;
class FuncDefNode;
class FuncBlockNode;
class StmtNode;
class ExprNode;

enum class DataType { INT };
enum class StmtType { RETURN };
enum class ExprType { BINARY, UNARY, NUMBER };

namespace koopa{
    
class IRBuilder {
public:
    explicit IRBuilder(Program& program) : program_(program) {}
    
    Program& program_;
    Function* current_function_{nullptr};
    BasicBlock* current_block_{nullptr};
};
}

class BaseNode {

public:
    virtual ~BaseNode() = default;
    virtual void PrintAST(size_t indent) const = 0;
};

class CompUnitNode : public BaseNode {

public:
    explicit CompUnitNode(std::unique_ptr<FuncDefNode> nd) : func_def_(std::move(nd)) {}
    void PrintAST(size_t indent) const override;
    void GenerateIR(koopa::IRBuilder& builder) const;
    
    std::unique_ptr<FuncDefNode> func_def_;

};

class FuncDefNode : public BaseNode {

public:
    explicit FuncDefNode(DataType type, std::string ident, std::unique_ptr<FuncBlockNode> block) : type_(type), ident_(std::move(ident)), block_(std::move(block)) {}
    void PrintAST(size_t indent) const override;
    void GenerateIR(koopa::IRBuilder& builder) const;

    DataType type_;
    std::string ident_;
    std::unique_ptr<FuncBlockNode> block_;

};


class FuncBlockNode : public BaseNode {

public:
    explicit FuncBlockNode(std::unique_ptr<StmtNode> stmt){
        statements_.push_back(std::move(stmt));
    }

    void PrintAST(size_t indent) const override;
    void GenerateIR(koopa::IRBuilder& builder) const;

    std::vector<std::unique_ptr<StmtNode>> statements_;
};


class StmtNode : public BaseNode {

public:
    StmtNode(StmtType type) : type_(type) {}
    virtual void GenerateIR(koopa::IRBuilder& builder) const = 0;

    StmtType type_;
};

class RetStmt : public StmtNode {

public:
    explicit RetStmt(std::unique_ptr<ExprNode> expr) : StmtNode(StmtType::RETURN), expr_(std::move(expr)) {}
    void PrintAST(size_t indent) const override;
    void GenerateIR(koopa::IRBuilder& builder) const override;

    std::unique_ptr<ExprNode> expr_;
};

enum class UnaryOP{ POS, NEG, NOT };
enum class BinaryOP{ ADD, SUB, MUL, DIV, MOD, LS, GT, LE, GE, EQ, NE, AND, OR };

class ExprNode : public BaseNode {

public:
    ExprNode(ExprType type) : type_(type) {}
    virtual koopa::ValueID GenerateIR(koopa::IRBuilder& builder) const = 0;

    ExprType type_;
};

class NumExpr : public ExprNode {

public:
    explicit NumExpr(int num) : ExprNode(ExprType::NUMBER), number_(num) {}
    void PrintAST(size_t indent) const override;
    koopa::ValueID GenerateIR(koopa::IRBuilder& builder) const override;

    int number_;
};

class UnaryExpr : public ExprNode {

public:
    explicit UnaryExpr(UnaryOP op, std::unique_ptr<ExprNode> expr) : ExprNode(ExprType::UNARY), op_(op), operand_(std::move(expr)) {}
    void PrintAST(size_t indent) const override;
    koopa::ValueID GenerateIR(koopa::IRBuilder& builder) const override;

    UnaryOP op_;
    std::unique_ptr<ExprNode> operand_;
};

class BinaryExpr : public ExprNode {
    public:
    explicit BinaryExpr(BinaryOP op, std::unique_ptr<ExprNode> lhs, std::unique_ptr<ExprNode> rhs) : ExprNode(ExprType::BINARY), op_(op), lhs_(std::move(lhs)), rhs_(std::move(rhs)){}
    void PrintAST(size_t indent) const override;
    koopa::ValueID GenerateIR(koopa::IRBuilder& builder) const override;

    BinaryOP op_;
    std::unique_ptr<ExprNode> lhs_;
    std::unique_ptr<ExprNode> rhs_;
};

#endif //AST_HPP 

//CompUnit      ::= [CompUnit] (Decl | FuncDef);
//
//Decl          ::= ConstDecl | VarDecl;
//ConstDecl     ::= "const" BType ConstDef {"," ConstDef} ";";
//BType         ::= "int";
//ConstDef      ::= IDENT {"[" ConstExp "]"} "=" ConstInitVal;
//ConstInitVal  ::= ConstExp | "{" [ConstInitVal {"," ConstInitVal}] "}";
//VarDecl       ::= BType VarDef {"," VarDef} ";";
//VarDef        ::= IDENT {"[" ConstExp "]"}
//                | IDENT {"[" ConstExp "]"} "=" InitVal;
//InitVal       ::= Exp | "{" [InitVal {"," InitVal}] "}";
//
//FuncDef       ::= FuncType IDENT "(" [FuncFParams] ")" Block;
//FuncType      ::= "void" | "int";
//FuncFParams   ::= FuncFParam {"," FuncFParam};
//FuncFParam    ::= BType IDENT ["[" "]" {"[" ConstExp "]"}];
//
//Block         ::= "{" {BlockItem} "}";
//BlockItem     ::= Decl | Stmt;
//Stmt          ::= LVal "=" Exp ";"
//                | [Exp] ";"
//                | Block
//                | "if" "(" Exp ")" Stmt ["else" Stmt]
//                | "while" "(" Exp ")" Stmt
//                | "break" ";"
//                | "continue" ";"
//                | "return" [Exp] ";";
//
//Exp           ::= LOrExp;
//LVal          ::= IDENT {"[" Exp "]"};
//PrimaryExp    ::= "(" Exp ")" | LVal | Number;
//Number        ::= INT_CONST;
//UnaryExp      ::= PrimaryExp | IDENT "(" [FuncRParams] ")" | UnaryOp UnaryExp;
//UnaryOp       ::= "+" | "-" | "!";
//FuncRParams   ::= Exp {"," Exp};
//MulExp        ::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp;
//AddExp        ::= MulExp | AddExp ("+" | "-") MulExp;
//RelExp        ::= AddExp | RelExp ("<" | ">" | "<=" | ">=") AddExp;
//EqExp         ::= RelExp | EqExp ("==" | "!=") RelExp;
//LAndExp       ::= EqExp | LAndExp "&&" EqExp;
//LOrExp        ::= LAndExp | LOrExp "||" LAndExp;
//ConstExp      ::= Exp;
//符号	含义	符号	含义
//CompUnit	编译单元	Decl	声明
//ConstDecl	常量声明	BType	基本类型
//ConstDef	常数定义	ConstInitVal	常量初值
//VarDecl	变量声明	VarDef	变量定义
//InitVal	变量初值	FuncDef	函数定义
//FuncType	函数类型	FuncFParams	函数形参表
//FuncFParam	函数形参	Block	语句块
//BlockItem	语句块项	Stmt	语句
//Exp	表达式	LVal	左值表达式
//PrimaryExp	基本表达式	Number	数值
//UnaryExp	一元表达式	UnaryOp	单目运算符
//FuncRParams	函数实参表	MulExp	乘除模表达式
//AddExp	加减表达式	RelExp	关系表达式
//EqExp	相等性表达式	LAndExp	逻辑与表达式
//LOrExp	逻辑或表达式	ConstExp	常量表达式