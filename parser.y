%{
    #include "c2ir.hpp"
    #include <vector.h>
    #include <cstdio>
    #include <cstdlib>
    Block *programBLock;
    
    extern int yylex();
    void yyerror(const char *s)
    {
        printf("ERROR: %s:%d: %s\n", __FILE__, __LINE__, s);
    }
%}

%union {
    Node *node;
    std::string *string;
    
    Identifier *ident;
    Expression *expr;
    std::vector<VariableDeclaration*> *varvec;
    std::vector<Expression*> *exprvec;
    Block *block;
    Statement *stmt;
    VariableDeclaration *var_decl;
    int token;
}

%token <string>     T_identifier T_integer T_float
%token <string>     T_string T_char
%token <token>      T_comma
%token <token>      T_left_paren T_right_paren
%token <token>      T_left_curly T_right_curly
%token <token>      T_left_square T_right_square
%token <token>      T_mult T_div T_mod
%token <token>      T_bit_and T_bit_or T_bit_xor T_bit_not
%token <token>      T_log_and T_log_or T_log_not
%token <token>      T_lt T_gt T_le T_ge T_eq T_neq
%token <token>      T_dot T_arrow
%token <token>      T_plus T_minus
%token <token>      T_plus_seq T_minus_seq T_or_seq T_and_seq
%token <token>      T_inc T_dec
%token <token>      T_question T_colon T_seq_point

%type <ident> ident
%type <expr> numeric expr 
%type <varvec> func_decl_args
%type <exprvec> call_args
%type <block> program stmts block
%type <stmt> stmt var_decl func_decl
%type <token> comparison


/* operater precendence */
%left T_inc T_dec T_left_paren T_right_paren T_left_square T_right_square T_dot T_arrow /* FIXME: compound literal */
%left T_log_not /* FIXME: type cast, indirection, address of, sizeof alignof */
%left T_mult T_div T_mod
%left T_plus T_minus

%start program

%%

program : stmts { programBlock = $1; }
        ;
        
stmts : stmt { $$ = new Block(); $$->statements.push_back($<stmt>1); }
      | stmts stmt { $1->statements.push_back($<stmt>2); }
      ;

stmt : var_decl | func_decl
     | expr { $$ = new ExpressionStatement(*$1); }
     ;

block : T_left_curly stmts T_right_curly { $$ = $2; }
      | T_left_curly T_right_curly { $$ = new Block(); }
      ;

var_decl : ident ident { $$ = new VariableDeclaration(*$1, *$2); }
         | ident ident T_eq expr { $$ = new VariableDeclaration(*$1, *$2, $4); }
         ;
        
func_decl : ident ident T_left_paren func_decl_args T_right_paren block 
            { $$ = new FunctionDeclaration(*$1, *$2, *$4, *$6); delete $4; }
          ;
    
func_decl_args : { $$ = new VariableList(); }
          | var_decl { $$ = new VariableList(); $$->push_back($<var_decl>1); }
          | func_decl_args T_comma var_decl { $1->push_back($<var_decl>3); }
          ;

ident : T_identifier { $$ = new Identifier(*$1); delete $1; }
      ;

numeric : T_integer { $$ = new Integer(atol($1->c_str())); delete $1; }
        | T_float { $$ = new Double(atof($1->c_str())); delete $1; }
        ;
    
expr : ident T_eq expr { $$ = new Assignment(*$<ident>1, *$3); }
     | ident T_left_paren call_args T_right_paren { $$ = new MethodCall(*$1, *$3); delete $3; }
     | ident { $<ident>$ = $1; }
     | numeric
     | expr comparison expr { $$ = new BinaryOperator(*$1, $2, *$3); }
     | T_left_paren expr T_right_paren { $$ = $2; }
     ;
    
call_args : { $$ = new ExpressionList(); }
          | expr { $$ = new ExpressionList(); $$->push_back($1); }
          | call_args T_comma expr  { $1->push_back($3); }
          ;

comparison : T_eq | T_neq | T_lt | T_gt | T_le | T_ge
           | T_plus | T_minus | T_mult | T_div | T_mod
           ;

%%

