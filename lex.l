%{   
    #include <string>
    #include "parser.hpp"

    #define LEX_STORE_STR_TOKEN(tkn) \
        yylval.string = new std::string(yytext, yyleng);\
        return T_##tkn
    #define LEX_TOKEN(tkn) return (yylval.token = T_##tkn)

    extern "C" int yywrap() { }
%}
   
%%

[ \t\n]                         ;
[a-zA-Z_][a-zA-Z0-9_]*          LEX_STORE_STR_TOKEN(identifier);
[0-9]+.[0-9]*                   LEX_STORE_STR_TOKEN(integer); /* FIXME: float type */
[0-9]+                          LEX_STORE_STR_TOKEN(float);
"\""[a-zA-Z_][a-zA-Z0-9_]*"\""  LEX_STORE_STR_TOKEN(string);
"\'"[a-zA-Z0-9_]"\'"            LEX_STORE_STR_TOKEN(char);
"."                             LEX_TOKEN(comma);
"("                             LEX_TOKEN(left_paren);
")"                             LEX_TOKEN(right_paren);
"{"                             LEX_TOKEN(left_curly);
"}"                             LEX_TOKEN(right_curly);
"["                             LEX_TOKEN(left_square);
"]"                             LEX_TOKEN(right_square);
"*"                             LEX_TOKEN(mult); /* FIXME: deference */
"/"                             LEX_TOKEN(div);
"%"                             LEX_TOKEN(mod);
"&"                             LEX_TOKEN(bit_and); /* FIXME: address of */
"|"                             LEX_TOKEN(bit_or);
"^"                             LEX_TOKEN(bit_xor);
"~"                             LEX_TOKEN(bit_not);
"&&"                            LEX_TOKEN(log_and);
"||"                            LEX_TOKEN(log_or);
"!"                             LEX_TOKEN(log_not);
"<"                             LEX_TOKEN(lt);
">"                             LEX_TOKEN(gt);
"<="                            LEX_TOKEN(le);
">="                            LEX_TOKEN(ge);
"=="                            LEX_TOKEN(eq);
"!="                            LEX_TOKEN(neq);
"."                             LEX_TOKEN(dot); /* FIXME */
"->"                            LEX_TOKEN(arrow); /* FIXME: */
"+"                             LEX_TOKEN(plus);
"-"                             LEX_TOKEN(minus);
"+="                            LEX_TOKEN(plus_seq); /* FIXME: */
"-="                            LEX_TOKEN(minus_seq); /* FIXME: */
"|="                            LEX_TOKEN(or_seq);
"&="                            LEX_TOKEN(and_seq);
"++"                            LEX_TOKEN(inc);
"--"                            LEX_TOKEN(dec);
"?"                             LEX_TOKEN(question);
":"                             LEX_TOKEN(colon);
";"                             LEX_TOKEN(seq_point);
.                               printf("Unknown token"); yyterminate();

%%
