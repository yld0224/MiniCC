%{

#include <iostream>
#include <memory>
#include <string>

int yylex();
void yyerror(std::unique_ptr<std::string>& ast, const char* s);

%}

%code requires {
    #include <memory>
    #include <string>
}

%parse-param { std::unique_ptr<std::string> &ast }

%union {
    std::string* str_val;
    int int_val;
}

%token INT
%token RETURN
%token <int_val> INT_CONST
%token <str_val> IDENT

%type <str_val> FuncDef FuncType Block Stmt Number

%%

CompUnit 
    : FuncDef {
        ast = std::unique_ptr<std::string>($1);
    }
    ;

FuncDef
    : FuncType IDENT '(' ')' Block {
        const auto type = std::unique_ptr<std::string>($1);
        const auto id = std::unique_ptr<std::string>($2);
        const auto block = std::unique_ptr<std::string>($5);
        $$ = new std::string(*type + " " + *id + "() " + *block);
    }
    ;

FuncType
    : INT {
        $$ = new std::string("int");
    }
    ;

Block
    : '{' Stmt '}' {
        const auto stmt = std::unique_ptr<std::string>($2);
        $$ = new std::string("{ " + *stmt + " }");
    }
    ;

Stmt
    : RETURN Number ';' {
        auto number = std::unique_ptr<std::string>($2);
        $$ = new std::string("return " + *number + ";");
    }
    ;

Number:
    INT_CONST {
        $$ = new std::string(std::to_string($1));
    }
    ;


%%

void yyerror(std::unique_ptr<std::string>& ast, const char* s) {
    std::cerr << "error: " << s << std::endl;
}
