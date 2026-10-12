%{

#include <cstdlib>
#include <string>

#include "sysy.tab.hpp"

%}

%option noyywrap
%option nounput
%option noinput

%x COMMENT

WhiteSpace      [ \n\t\r]+
LineComment     "//".*

Identifier      [a-zA-Z_][a-zA-Z0-9_]*

Decimal         [1-9][0-9]*
Octal           0[0-7]*
Hexadecimal     0[xX][0-9a-fA-F]+

Singleton       .

%%

"/*"            { BEGIN(COMMENT); }
<COMMENT>"*/"   { BEGIN(INITIAL); }
<COMMENT>.      {}
<COMMENT>\n     {}

{WhiteSpace}    {}
{LineComment}   {}

"int"           { return INT; }
"return"        { return RETURN; }
"<="            { return LE; }
">="            { return GE; }
"=="            { return EQ; }
"!="            { return NE; }
"&&"            { return AND; }
"||"            { return OR; }

{Identifier}    { yylval.str_val = new std::string(yytext); return IDENT;}
{Decimal}       { yylval.int_val = std::strtol(yytext, nullptr, 0); return INT_CONST; }
{Octal}         { yylval.int_val = std::strtol(yytext, nullptr, 0); return INT_CONST; }
{Hexadecimal}   { yylval.int_val = std::strtol(yytext, nullptr, 0); return INT_CONST; }
{Singleton}     { return yytext[0]; }

%%
