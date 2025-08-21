%{
	# include <stdio.h>
	# include <stdlib.h>
	# include <string.h>

	extern int  yylex();
	extern int  yylex_destroy(void);

	extern int  yyparse();
	extern FILE *yyin;

	int yyerror(const char *s);
    
    # define B_NO_PREFIX
    # include <btypes.h>
    # include <bcontext.h>
    # include <bsymbol.h>
    # include <bdecl.h>
%}

%code requires
{
	#include <btypes.h>
}

%union
{
	int			n;
	String		s;
	Expr		e;
}

%token<n>	CONSTANT
%token<s>	CHAR_CONSTANT
%token<s>	STR_CONSTANT
%token<s>	NAME

%token		AUTO EXTERN WHILE RETURN IF ELSE SWITCH CASE DEFAULT BREAK GOTO
%token		F_CHAR F_LCHAR
%token		SEMI COMMA
%token		LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET
%token		QUESTION COLON

%token		ASSIGN
%token		ASSIGN_EQ ASSIGN_NE ASSIGN_LE ASSIGN_GE ASSIGN_LT ASSIGN_GT
%token		ASSIGN_LSHIFT ASSIGN_RSHIFT
%token		ASSIGN_OR ASSIGN_AND
%token		ASSIGN_PLUS ASSIGN_MINUS ASSIGN_MULT ASSIGN_DIV ASSIGN_MOD
%token		OR XOR
%token		AND NOT
%token		EQ NE
%token		LT LE GT GE
%token		LSHIFT RSHIFT
%token		INCR DECR
%token		PLUS MINUS
%token		MULT DIV MOD

%right		ASSIGN
%right		ASSIGN_EQ ASSIGN_NE ASSIGN_LE ASSIGN_GE ASSIGN_LT ASSIGN_GT
%right		ASSIGN_LSHIFT ASSIGN_RSHIFT
%right		ASSIGN_OR ASSIGN_AND
%right		ASSIGN_PLUS ASSIGN_MINUS ASSIGN_MULT ASSIGN_DIV ASSIGN_MOD
%left		OR
%left		XOR
%left		AND NOT
%left		EQ NE
%left		LT LE GT GE
%left		LSHIFT RSHIFT
%left		PLUS MINUS
%left		MULT DIV MOD

%right		UMINUS UAMP USTAR UNOT UINCR UDECR
%left		POSTFIX

%nonassoc	LOWER_THAN_ELSE
%nonassoc	ELSE

%type<s>	param

%type<e>	expr
%type<e>	expr_assignment
%type<e>	expr_conditional
%type<e>	expr_logical_or
%type<e>	expr_logical_xor
%type<e>	expr_logical_and
%type<e>	expr_equality
%type<e>	expr_relational
%type<e>	expr_shift
%type<e>	expr_additive
%type<e>	expr_multiplicative
%type<e>	expr_unary
%type<e>	expr_postfix
%type<e>	expr_builtin
%type<e>	expr_primary
%type<e>	argument_list
%type<e>	constant

%%

program
	: { B_program_start(); }
		definition_list
	  { B_program_stop(); }
	;

definition_list
	: definition_list definition
	| definition
	;

definition
	: function
	;

function
	: NAME
        { function_start($1); }
	  LPAREN param_list_opt RPAREN compound_statement
        { function_stop(); }
	;

param_list_opt
	: param_list
	| 
	;

param_list
	: param
	| param_list COMMA param
	;

param
	: NAME
	;

compound_statement
	: LBRACE RBRACE
	| LBRACE 
		statement_list
	  RBRACE
	;

statement_list
	: statement_list statement
	| statement
	;

statement
	: AUTO auto_decl_list SEMI
	| EXTERN extrn_decl_list SEMI
	| expr SEMI
	| if_statement
	| WHILE 
      LPAREN expr RPAREN 
      statement
	| RETURN LPAREN expr RPAREN SEMI
	| RETURN SEMI
	| SEMI
	| compound_statement
	;

auto_decl_list
	: auto_decl_list COMMA auto_decl
	| auto_decl
	;

auto_decl
	: NAME
	;

extrn_decl_list
	: extrn_decl_list COMMA extrn_decl
	| extrn_decl
	;

extrn_decl
	: NAME
	;

if_statement
	: IF LPAREN expr RPAREN statement %prec LOWER_THAN_ELSE
	| IF LPAREN expr RPAREN statement ELSE statement
	;

expr
	: expr_assignment	
	;

expr_assignment
	: expr_conditional
	| expr_assignment ASSIGN expr_assignment
    | expr_assignment ASSIGN_PLUS expr_assignment
    | expr_assignment ASSIGN_MINUS expr_assignment
    | expr_assignment ASSIGN_MULT expr_assignment
    | expr_assignment ASSIGN_DIV expr_assignment
    | expr_assignment ASSIGN_MOD expr_assignment
    | expr_assignment ASSIGN_AND expr_assignment
    | expr_assignment ASSIGN_OR expr_assignment
    | expr_assignment ASSIGN_LT expr_assignment
    | expr_assignment ASSIGN_GT expr_assignment
    | expr_assignment ASSIGN_LE expr_assignment
    | expr_assignment ASSIGN_GE expr_assignment
    | expr_assignment ASSIGN_EQ expr_assignment
    | expr_assignment ASSIGN_NE expr_assignment
    | expr_assignment ASSIGN_LSHIFT expr_assignment
    | expr_assignment ASSIGN_RSHIFT expr_assignment
    ;

expr_conditional
	: expr_logical_or
	| expr_logical_or QUESTION expr COLON expr_conditional
	;

expr_logical_or
	: expr_logical_xor
	| expr_logical_or OR expr_logical_xor
	;

expr_logical_xor
	: expr_logical_and
	| expr_logical_xor XOR expr_logical_and
	;

expr_logical_and
	: expr_equality
	| expr_logical_and AND expr_equality
	;

expr_equality
	: expr_relational
	| expr_equality EQ expr_relational
	| expr_equality NE expr_relational
	;

expr_relational
	: expr_shift
	| expr_relational LT expr_shift
	| expr_relational LE expr_shift
	| expr_relational GT expr_shift
	| expr_relational GE expr_shift
	;

expr_shift
	: expr_additive
	| expr_shift LSHIFT expr_additive
	| expr_shift RSHIFT expr_additive
	;

expr_additive
	: expr_multiplicative
	| expr_additive PLUS expr_multiplicative
	| expr_additive MINUS expr_multiplicative
	;

expr_multiplicative
	: expr_unary
	| expr_multiplicative MULT expr_unary
	| expr_multiplicative DIV expr_unary
	| expr_multiplicative MOD expr_unary
	;

expr_unary
	: expr_postfix
	| MULT expr_unary %prec USTAR
        { $$ = $2; }
	| AND expr_postfix %prec UAMP
        { $$ = $2; }
	| MINUS expr_unary %prec UMINUS
        { $$ = $2; }
	| NOT expr_unary %prec UNOT
        { $$ = $2; }
	| INCR expr_unary
        { $$ = $2; }
	| DECR expr_unary
        { $$ = $2; }
	;

expr_postfix
	: expr_primary
	| expr_builtin
	| expr_postfix INCR
	| expr_postfix DECR
	| expr_postfix LBRACKET expr RBRACKET
	| expr_postfix LPAREN argument_list RPAREN
	| expr_postfix LPAREN RPAREN
	;

expr_builtin
	: F_CHAR LPAREN expr COMMA expr RPAREN
        { $$ = $3; }
	| F_LCHAR LPAREN expr COMMA expr COMMA expr RPAREN
        { $$ = $3; }
	;

expr_primary
	: NAME
        { $$ = (Expr){ .type = EXPR_VARIABLE, .name = $1 }; }
	| LPAREN expr RPAREN
        { $$ = $2; }
	| constant
	;

argument_list
	: expr
	| argument_list COMMA expr
	;

constant
	: CONSTANT
        { $$ = (Expr){ .type = EXPR_IMMEDIATE, .imm = $1 }; }
	| STR_CONSTANT
        { $$ = (Expr){ .type = EXPR_ROSTRING, .ptr = $1 }; }
	| CHAR_CONSTANT
        { $$ = (Expr){ .type = EXPR_IMMEDIATE, .imm = 0 }; }
	;

%%
