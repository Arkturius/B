%{
	# include <stdio.h>
	# include <stdlib.h>
	# include <string.h>

	# include <b.h>

	extern int  yylex();
	extern int  yylex_destroy(void);

	extern int  yyparse();
	extern FILE *yyin;

	int yyerror(const char *s);
%}

%code requires
{
	#include <b.h>
}

%union
{
	int			n;
	String		s;
	Expression	e;
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

%token<n>	ASSIGN
%token<n>	ASSIGN_EQ ASSIGN_NE ASSIGN_LE ASSIGN_GE ASSIGN_LT ASSIGN_GT
%token<n>	ASSIGN_LSHIFT ASSIGN_RSHIFT
%token<n>	ASSIGN_OR ASSIGN_AND
%token<n>	ASSIGN_PLUS ASSIGN_MINUS ASSIGN_MULT ASSIGN_DIV ASSIGN_MOD
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
%type<e>	argument
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
		{ B_function_start($1); }
	  LPAREN param_list_opt RPAREN statement
		{ B_function_stop($1); }
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
		{ B_function_param($1); }
	;

compound_statement
	: LBRACE RBRACE
	| LBRACE 
		{ B_scope_start(); }
	  statement_list
		{ B_scope_stop(); }
	  RBRACE
	;

statement_list
	: statement_list statement
	| statement
	;

statement
	: AUTO auto_decl_list SEMI
		{ B_auto_decl(); }
	| EXTERN extrn_decl_list SEMI
	| expr SEMI
	| if_statement
	| WHILE 
		{ B_while_start(); }
      LPAREN expr RPAREN 
		{ B_while_condition($4); }
      statement
		{ B_while_stop(); }
	| switch_statement
	| label_statement
	| GOTO NAME SEMI
		{ B_goto($2); }
	| RETURN LPAREN expr RPAREN SEMI
		{ B_return_expr($3); }
	| RETURN SEMI
		{ B_return_expr((Expression){0}); }
	| SEMI
	| compound_statement
	;

auto_decl_list
	: auto_decl_list COMMA auto_decl
	| auto_decl
	;

auto_decl
	: NAME
		{ B_auto_variable($1, WORD_SIZE); }
	| NAME LBRACKET CONSTANT RBRACKET
		{ B_auto_variable($1, WORD_SIZE * ($3 + 1)); }
	;

extrn_decl_list
	: extrn_decl_list COMMA extrn_decl
	| extrn_decl
	;

extrn_decl
	: NAME
		{ B_extern_variable($1); }
	;

if_statement
	: if_start statement %prec LOWER_THAN_ELSE
		{ B_if_stop(false); }
	| if_start statement ELSE 
		{ B_if_stop(true); }
	  statement
		{ B_else_stop(); }
	;

if_start
	: IF LPAREN expr RPAREN
		{ B_if_start($3); }
	;

switch_statement
	: SWITCH LPAREN expr RPAREN statement
	;

label_statement
	: CASE constant COLON statement
	| DEFAULT COLON statement
	| NAME COLON statement
	;

expr
	: expr_assignment	
	;

expr_assignment
	: expr_conditional
	| expr_assignment ASSIGN		expr_assignment
		{ $$ = B_expression_assignment(ASSIGN_OP, $1, $3); }
    | expr_assignment ASSIGN_PLUS	expr_assignment
		{ $$ = B_expression_assignment(ASSIGN_OP_PLUS, $1, $3); }
    | expr_assignment ASSIGN_MINUS	expr_assignment
		{ $$ = B_expression_assignment(ASSIGN_OP_MINUS, $1, $3); }
    | expr_assignment ASSIGN_MULT	expr_assignment
    | expr_assignment ASSIGN_DIV	expr_assignment
    | expr_assignment ASSIGN_MOD	expr_assignment
    | expr_assignment ASSIGN_AND	expr_assignment
    | expr_assignment ASSIGN_OR		expr_assignment
    | expr_assignment ASSIGN_LT 	expr_assignment
    | expr_assignment ASSIGN_GT 	expr_assignment
    | expr_assignment ASSIGN_LE 	expr_assignment
    | expr_assignment ASSIGN_GE 	expr_assignment
    | expr_assignment ASSIGN_EQ 	expr_assignment
    | expr_assignment ASSIGN_NE 	expr_assignment
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
		{ $$ = B_expression_binop(BINOP_AND, $1, $3); }
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
		{ $$ = B_expression_binop(BINOP_PLUS, $1, $3); }
	| expr_additive MINUS expr_multiplicative
		{ $$ = B_expression_binop(BINOP_MINUS, $1, $3); }
	;

expr_multiplicative
	: expr_unary
	| expr_multiplicative MULT expr_unary
	| expr_multiplicative DIV expr_unary
	| expr_multiplicative MOD expr_unary
	;

/* TODO: Handle expression passing, those are to avoid type clashes. */
expr_unary
	: expr_postfix
	| MULT expr_unary %prec USTAR		
		{ $$ = B_expression_deref($2); }
	| AND expr_postfix %prec UAMP
		{ $$ = B_expression_address($2); }
	| MINUS expr_unary %prec UMINUS		{ $$ = $2; }
	| NOT expr_unary %prec UNOT			{ $$ = $2; }
	| INCR expr_unary
		{ $$ = B_expression_assignment(ASSIGN_OP_PLUS, $2, IMM(1)); }
	| DECR expr_unary
		{ $$ = B_expression_assignment(ASSIGN_OP_PLUS, $2, IMM(1)); }
	;

expr_postfix
	: expr_primary
	| expr_builtin
	| expr_postfix INCR
		{ $$ = B_expression_incr($1); }
	| expr_postfix DECR
		{ $$ = B_expression_decr($1); }
	| expr_postfix LBRACKET expr RBRACKET
		{ $$ = B_expression_subscript($1, $3); }
	| expr_postfix call_start LPAREN argument_list RPAREN
		{ $$ = B_function_call($1); }
	| expr_postfix call_start LPAREN RPAREN
		{ $$ = B_function_call($1); }
	;

call_start
	: { B_function_invoke(); }
	;

expr_builtin
	: F_CHAR LPAREN expr COMMA expr RPAREN				{ $$ = B_builtin_char($3, $5); }
	| F_LCHAR LPAREN expr COMMA expr COMMA expr RPAREN	{ $$ = $3; }
	;

expr_primary
	: NAME					{ $$ = B_expression_variable($1); }
	| LPAREN expr RPAREN	{ $$ = $2; }
	| constant
	;

argument_list
	: argument
	| argument_list COMMA argument
	;

argument
	: expr
		{ B_function_argument($1); }
	;

constant
	: CONSTANT		{ $$ = B_expression_constant($1, NULL, false); }
	| STR_CONSTANT	{ $$ = B_expression_constant(0,  $1,   false); }
	| CHAR_CONSTANT	{ $$ = B_expression_constant(0,  $1,   true);  }
	;

%%
