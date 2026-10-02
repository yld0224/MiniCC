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

class BaseNode {

public:
    virtual ~BaseNode() = default;

};

class CompUnitNode : public BaseNode {

public:
    std::unique_ptr<FuncDefNode> func_def_;

};

enum class DataType { INT };
enum class StmtType { RETURN };
enum class ExprType { NUMBER };

class FuncDefNode : public BaseNode {

public:
    DataType type_;
    std::string ident_;
    std::unique_ptr<FuncBlockNode> block_;

};


class FuncBlockNode : public BaseNode {

public:
    std::vector<std::unique_ptr<StmtNode>> statements_;
};


class StmtNode : public BaseNode {

public:
    StmtNode(StmtType type) : type_(type) {}
    virtual ~StmtNode() = default;

    StmtType type_;
};

class RetStmt : public StmtNode {

public:
    explicit RetStmt(std::unique_ptr<ExprNode> expr) : StmtNode(StmtType::RETURN), expr_(std::move(expr)) {}
    std::unique_ptr<ExprNode> expr_;
};


class ExprNode : public BaseNode {

public:
    ExprNode(ExprType type) : type_(type) {}
    virtual ~ExprNode() = default;

    ExprType type_;
};

class NumExpr : public ExprNode {

public:
    NumExpr(int num) : ExprNode(ExprType::NUMBER), number_(num) {}

    int number_;
};

#endif //AST_HPP 