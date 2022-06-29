%{   
    #include <cstdio>
    #include <string>
    #include "ASTnode.hpp"
    #include "parser.hpp"

    #define LEX_TOKEN(tkn)                  \
        do {                                \
            puts("    LEX_" #tkn);          \
            return (yylval.token = T_##tkn);\
        } while (0)

    #define LEX_STORE_STR_TOKEN(tkn)                        \
        do {                                                \
            yylval.string = new std::string(yytext, yyleng);\
            puts("    LEX_" #tkn);                          \
            return T_##tkn;                                 \
        } while (0)
%}

%option noyywrap
 
%%

[ \t\n]                         ;
"extern"                        { LEX_TOKEN(EXTERN); }
"int"                           { LEX_STORE_STR_TOKEN(INT); }
"char"                          { LEX_STORE_STR_TOKEN(CHAR); }
"return"                        { LEX_TOKEN(RETURN); }
"#include <"[a-zA-Z0-9.]+">"    { LEX_STORE_STR_TOKEN(HEADER); }
\"[ a-zA-Z0-9,.!]+\"            { LEX_STORE_STR_TOKEN(LITERAL); }
[a-zA-Z_][a-zA-Z0-9_]*          { LEX_STORE_STR_TOKEN(IDENTIFIER); }
[0-9]+                          { LEX_STORE_STR_TOKEN(INTEGER); }
"*"                             { LEX_TOKEN(ASTERISK); }
"+"                             { LEX_TOKEN(ADD); }
"-"                             { LEX_TOKEN(MINUS); }
"="                             { LEX_TOKEN(EQUAL); } 
"=="                            { LEX_TOKEN(CMP_EQUAL); }
","                             { LEX_TOKEN(COMMA); }
"("                             { LEX_TOKEN(LPAREN); }
")"                             { LEX_TOKEN(RPAREN); }
"{"                             { LEX_TOKEN(LBRACE); }
"}"                             { LEX_TOKEN(RBRACE); }
";"                             { LEX_TOKEN(SEQPOINT); }
.                               { printf("Unknown token"); yyterminate(); }

%%
