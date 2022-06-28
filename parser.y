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
    std::string *string;
    
    NIdentifier *ident;
    NExpression *expr;
    vector<shared_ptr<NVariableDeclaration>> *varvec;
    vector<shared_ptr<NExpression>> *exprvec;
    NBlock *block;
    NStatement *stmt;
    NVariableDeclaration *var_decl;
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

%type <block> program stmts block
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

program             : func_decl
                    | T_HEADER
                    | program func_decl
                    ;

func_decl           : T_EXTERN typename ident T_LPAREN func_decl_args T_RPAREN T_SEQPOINT
                    | typename ident T_LPAREN func_decl_args T_RPAREN block
                    ;

call_args           : 
                    | ident
                    | call_args T_COMMA ident
                    ;

func_decl_args      : 
                    | func_decl_args T_COMMA var_decl
                    | var_decl
                    ;

var_decl            : typename ident
                    | typename ident T_EQUAL expr
                    ;

block               : T_LBRACE stmts T_RBRACE
                    ;

stmts               : stmt
                    | stmts stmt
                    ;

stmt                : var_decl T_SEQPOINT
                    | expr T_SEQPOINT
                    | T_RETURN expr T_SEQPOINT
                    ;

expr                : 
                    | ident
                    | numeric
                    | T_LPAREN ident T_RPAREN
                    | ident T_LPAREN call_args T_RPAREN
                    | expr T_ADD expr 
                    | expr T_MINUS expr 
                    ;

ident               : T_IDENTIFIER
                    ;

typename            : T_INT
                    | T_CHAR
                    | T_CHAR T_ASTERISK
                    ;

numeric             : T_INTEGER
                    | T_LITERAL
                    ;

%%
