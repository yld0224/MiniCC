#ifndef AST_HPP
#define AST_HPP

#include <memory>
#include <string>
#include <vector>


class BaseNode;
class CompUnitNode;
class FuncDefNode;
class FuncBlockNode;
class StmtNode;
class ExprNode;

enum class DataType { INT };
enum class StmtType { RETURN };
enum class ExprType { NUMBER };

class BaseNode {

public:
    virtual ~BaseNode() = default;
    virtual void PrintAST(size_t indent) const = 0;
};

class CompUnitNode : public BaseNode {

public:
    explicit CompUnitNode(std::unique_ptr<FuncDefNode> nd) : func_def_(std::move(nd)) {}
    void PrintAST(size_t indent) const override;
    
    std::unique_ptr<FuncDefNode> func_def_;

};

class FuncDefNode : public BaseNode {

public:
    explicit FuncDefNode(DataType type, std::string ident, std::unique_ptr<FuncBlockNode> block) : type_(type), ident_(std::move(ident)), block_(std::move(block)) {}
    void PrintAST(size_t indent) const override;

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

    std::vector<std::unique_ptr<StmtNode>> statements_;
};


class StmtNode : public BaseNode {

public:
    StmtNode(StmtType type) : type_(type) {}

    StmtType type_;
};

class RetStmt : public StmtNode {

public:
    explicit RetStmt(std::unique_ptr<ExprNode> expr) : StmtNode(StmtType::RETURN), expr_(std::move(expr)) {}
    void PrintAST(size_t indent) const override;

    std::unique_ptr<ExprNode> expr_;
};


class ExprNode : public BaseNode {

public:
    ExprNode(ExprType type) : type_(type) {}

    ExprType type_;
};

class NumExpr : public ExprNode {

public:
    explicit NumExpr(int num) : ExprNode(ExprType::NUMBER), number_(num) {}
    void PrintAST(size_t indent) const override;

    int number_;
};

#endif //AST_HPP 