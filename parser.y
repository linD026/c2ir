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
%token <string>     T_STRING T_CHAR

%token <string>     T_INTEGER
%token <token>      T_ADD T_MINUS

%token <token>      T_EQUAL

%token <token>      T_CMP_EQUAL

%token <token>      T_COMMA T_LPAREN T_RPAREN T_LBRACE T_RBRACE
%token <token>      T_SEQPOINT
%token <token>      T_RETURN

%type <ident> ident
%type <expr> numeric expr 
%type <varvec> func_decl_args
%type <exprvec> call_args
%type <block> program stmts block
%type <stmt> stmt var_decl func_decl
%type <token> comparison

/* operater precendence */
%left T_ADD T_MINUS

%start program


// shared_ptr<NVariableDeclaration>($<var_decl>3)
%%

program : stmts { programBlock = $1; }
		;
		
stmts : stmt { $$ = new NBlock(); $$->statements->push_back(shared_ptr<NStatement>($1)); }
	  | stmts stmt { $1->statements->push_back(shared_ptr<NStatement>($2)); }
	  ;

stmt : var_decl | func_decl
	 | expr { $$ = new NExpressionStatement(shared_ptr<NExpression>($1)); }
	 | T_RETURN expr { $$ = new NReturnStatement(shared_ptr<NExpression>($2)); }
     ;

block : T_LBRACE stmts T_RBRACE { $$ = $2; }
	  | T_LBRACE T_RBRACE { $$ = new NBlock(); }
	  ;

var_decl : ident ident { $$ = new NVariableDeclaration(shared_ptr<NIdentifier>($1), shared_ptr<NIdentifier>($2), nullptr); }
		 | ident ident T_EQUAL expr { $$ = new NVariableDeclaration(shared_ptr<NIdentifier>($1), shared_ptr<NIdentifier>($2), shared_ptr<NExpression>($4)); }
		 ;

func_decl : ident ident T_LPAREN func_decl_args T_RPAREN block 
			{ $$ = new NFunctionDeclaration(shared_ptr<NIdentifier>($1), shared_ptr<NIdentifier>($2), shared_ptr<VariableList>($4), shared_ptr<NBlock>($6)); }
		  ;
	
func_decl_args : { $$ = new VariableList(); }
		       | var_decl { $$ = new VariableList(); $$->push_back(shared_ptr<NVariableDeclaration>($<var_decl>1)); }
		       | func_decl_args T_COMMA var_decl { $1->push_back(shared_ptr<NVariableDeclaration>($<var_decl>3)); }
		       ;

ident : T_IDENTIFIER { $$ = new NIdentifier(*$1); delete $1; }
	  ;

numeric : T_INTEGER { $$ = new NInteger(atol($1->c_str())); delete $1; }
		;
	
expr : ident T_EQUAL expr { $$ = new NAssignment(shared_ptr<NIdentifier>($1), shared_ptr<NExpression>($3)); }
	 | ident T_LPAREN call_args T_RPAREN { $$ = new NMethodCall(shared_ptr<NIdentifier>($1), shared_ptr<ExpressionList>($3)); }
	 | ident { $<ident>$ = $1; }
	 | numeric
         | expr T_ADD expr { $$ = new NBinaryOperator(shared_ptr<NExpression>($1), $2, shared_ptr<NExpression>($3)); }
         | expr T_MINUS expr { $$ = new NBinaryOperator(shared_ptr<NExpression>($1), $2, shared_ptr<NExpression>($3)); }
 	 | expr comparison expr { $$ = new NBinaryOperator(shared_ptr<NExpression>($1), $2, shared_ptr<NExpression>($3)); }
     | T_LPAREN expr T_RPAREN { $$ = $2; }
	 ;
	
call_args : { $$ = new ExpressionList(); }
		  | expr { $$ = new ExpressionList(); $$->push_back(shared_ptr<NExpression>($1)); }
		  | call_args T_COMMA expr { $1->push_back(shared_ptr<NExpression>($3)); }
		  ;

comparison : T_CMP_EQUAL ;

%%
