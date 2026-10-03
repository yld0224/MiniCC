%{

#include <iostream>
#include <memory>
#include <string>

#include "AST.hpp"

int yylex();
void yyerror(std::unique_ptr<CompUnitNode>& ast, const char* s);

%}

%code requires {
    #include <memory>
    #include <string>

    #include "AST.hpp"
}

%parse-param { std::unique_ptr<CompUnitNode> &ast }

%union {
    std::string* str_val;
    int int_val;
    BaseNode* ast_val;
    DataType type_val;
}

%token INT
%token RETURN
%token <int_val> INT_CONST
%token <str_val> IDENT

%type <ast_val> FuncDef Block Stmt Number
%type <type_val> FuncType

%%

CompUnit 
    : FuncDef {
        auto func_def = static_cast<FuncDefNode*>($1);
        
        ast = std::make_unique<CompUnitNode>(std::unique_ptr<FuncDefNode>(func_def));
    }
    ;

FuncDef
    : FuncType IDENT '(' ')' Block {
        auto func_block = static_cast<FuncBlockNode*>($5);
        auto ident = std::unique_ptr<std::string>($2);

        $$ = new FuncDefNode($1, std::move(*ident), std::unique_ptr<FuncBlockNode>(func_block));
    }
    ;

FuncType
    : INT {
        $$ = DataType::INT;
    }
    ;

Block
    : '{' Stmt '}' {
        auto stmt = static_cast<StmtNode*>($2);
        
        $$ = new FuncBlockNode(std::unique_ptr<StmtNode>(stmt));
    }
    ;

Stmt
    : RETURN Number ';' {
        auto numexpr = static_cast<ExprNode*>($2);

        $$ = new RetStmt(std::unique_ptr<ExprNode>(numexpr));
    }
    ;

Number:
    INT_CONST {
        $$ = new NumExpr($1);
    }
    ;


%%

void yyerror(std::unique_ptr<CompUnitNode>& ast, const char* s) {
    std::cerr << "error: " << s << std::endl;
}
