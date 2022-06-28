%{   
    #include <string>
    #include "ASTnode.hpp"
    #include "parser.hpp"

    #define LEX_STORE_STR_TOKEN(tkn) \
        yylval.string = new std::string(yytext, yyleng);\
        return T_##tkn
    #define LEX_TOKEN(tkn) return (yylval.token = T_##tkn)

    extern "C" int yywrap() { }
%}
   
%%

[ \t\n]                         ;
[a-zA-Z_][a-zA-Z0-9_]*          LEX_STORE_STR_TOKEN(IDENTIFIER);
"\""[a-zA-Z_][a-zA-Z0-9_]*"\""  LEX_STORE_STR_TOKEN(STRING);
"\'"[a-zA-Z0-9_]"\'"            LEX_STORE_STR_TOKEN(CHAR);

[0-9]+                          LEX_STORE_STR_TOKEN(INTEGER);
"+"                             LEX_TOKEN(ADD);
"-"                             LEX_TOKEN(MINUS);

"="                             LEX_TOKEN(EQUAL);

"=="                            LEX_TOKEN(CMP_EQUAL);

","                             LEX_TOKEN(COMMA);
"("                             LEX_TOKEN(LPAREN);
")"                             LEX_TOKEN(RPAREN);
"{"                             LEX_TOKEN(LBRACE);
"}"                             LEX_TOKEN(RBRACE);
";"                             LEX_TOKEN(SEQPOINT);

"return"                        LEX_TOKEN(RETURN);

.                               printf("Unknown token"); yyterminate();

%%
