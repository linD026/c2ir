%{
    #include "ASTnode.hpp"
    #include <vector>
    #include <cstdio>
    NBlock *programBlock;

    extern int yylex();
    void yyerror(const char *s)
    {
        printf("ERROR: %s:%d: %s\n", __FILE__, __LINE__, s);
    }
%}

%union {
    Node *node;
    
    NBlock *block;
    NStatement *stmt;
    NExpression *expr;
    
    NIdentifier *ident;
    NVariableDeclaration *var_decl;
    vector<NVariableDeclaration *> *varvec;
    vector<NExpression *> *exprvec;
    
    std::string *string;
    int token;
}

%token <string>     T_IDENTIFIER

%token <string>     T_INT T_CHAR
%token <string>     T_INTEGER T_LITERAL
%token <string>     T_HEADER

%token <token>      T_ADD T_MINUS T_ASTERISK
%token <token>      T_EQUAL
%token <token>      T_CMP_EQUAL

%token <token>      T_COMMA T_LPAREN T_RPAREN T_LBRACE T_RBRACE
%token <token>      T_SEQPOINT
%token <token>      T_EXTERN T_RETURN

%type <block> program program_unit stmts block
%type <stmt> stmt var_decl func_decl
%type <token> comparison
%type <expr> numeric expr 
%type <ident> ident typename

%type <varvec> func_decl_args
%type <exprvec> call_args

/* operater precendence */
%left T_ADD T_MINUS

%start program

%%

program             : program_unit { programBlock = $1; }
                    ;

program_unit        : func_decl
                    | T_HEADER
                    | program func_decl
                    ;

func_decl           : T_EXTERN typename ident T_LPAREN func_decl_args T_RPAREN T_SEQPOINT
                      { $$ = new NFunctionDeclaration($2, $3, $5, nullptr); }
                    | typename ident T_LPAREN func_decl_args T_RPAREN block
                      { $$ = new NFunctionDeclaration($1, $2, $4, $6); }
                    ;

call_args           : { $$ = new ExpressionList(); }
                    | ident { $$ = new ExpressionList(); $$->push_back($1); }
                    | call_args T_COMMA ident { $1->push_back($3); }
                    ;

func_decl_args      : { $$ = new VariableList(); } 
                    | func_decl_args T_COMMA var_decl { $1->push_back($<var_decl>3); }
                    | var_decl { $$ = new VariableList(); $$->push_back($<var_decl>1); }
                    ;

var_decl            : typename ident { $$ = new NVariableDeclaration($1, $2, nullptr); }
                    | typename ident T_EQUAL expr { $$ = new NVariableDeclaration($1, $2, $4); }
                    ;

block               : T_LBRACE stmts T_RBRACE { $$ = $2; }
                    | T_LBRACE T_RPAREN { $$ = new NBlock(); }
                    ;

stmts               : stmt { $$ = new NBlock(); /* $$->statements->push_back($1);*/ }
                    | stmts stmt { $1->statements->push_back($2); }
                    ;

stmt                : var_decl T_SEQPOINT
                    | expr T_SEQPOINT { $$ = new NExpressionStatement($1); }
                    | T_RETURN expr T_SEQPOINT { $$ = new NReturnStatement($2); }
                    ;

expr                : 
                    | ident { $<ident>$ = $1; }
                    | T_LPAREN ident T_RPAREN { $<ident>$ = $2; }
                    | numeric
                    | T_LITERAL { $$ = new NLiteral(*$1); delete $1; }
                    | ident T_LPAREN call_args T_RPAREN { $$ = new NMethodCall($1, $3); }
                    | ident T_EQUAL expr { $$ = new NAssignment($1, $3); }
                    | ident T_ADD expr { $$ = new NBinaryOperator($1, $2, $3); } 
                    | ident T_MINUS expr { $$ = new NBinaryOperator($1, $2, $3); } 
                    ;

ident               : T_IDENTIFIER { $$ = new NIdentifier(*$1); delete $1; }
                    ;

typename            : T_INT { $$ = new NIdentifier(*$1); $$->isType = true; delete $1; }
                    | T_CHAR { $$ = new NIdentifier(*$1); $$->isType = true; delete $1; }
                    | T_CHAR T_ASTERISK
                      { $$ = new NIdentifier(*$1); $$->isType = true; $$->isPtr = true; delete $1;}
                    ;

numeric             : T_INTEGER { $$ = new NInteger(atol($1->c_str())); }
                    ;

%%
