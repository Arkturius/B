%{
	# include <stdio.h>
	# include <stdlib.h>
	# include <string.h>

	# include <b.h>

	extern FILE 		*yyin;
	extern int			yylex();
	extern int			yyparse();
	extern int  		yylex_destroy(void);

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
%type<e>	expr_logical_and
%type<e>	expr_equality
%type<e>	expr_relational
%type<e>	expr_shift
%type<e>	expr_additive
%type<e>	expr_multiplicative
%type<e>	expr_unary
%type<e>	expr_postfix
%type<e>	expr_builtin_val
%type<e>	expr_primary
%type<e>	argument_list
%type<e>	argument
%type<e>	constant

%%

program
	: { B_control_program_start(); }
		definition_list
	  { B_control_program_stop(); }
	;

definition_list
	: definition_list definition
	| definition
	;

definition
	: function
	| global_def
	;

global_def
	: NAME LBRACKET CONSTANT RBRACKET ivals SEMI
	    { B_eval_vector_def($1, $3); }
	| NAME LBRACKET RBRACKET ivals SEMI
		{ B_eval_vector_def($1, 0); }
	| NAME ivals SEMI
		{ B_eval_simple_def($1); }
	;

ivals 
	: ival_list
	|
	;

ival_list
	: ival
	| ival_list COMMA ival
	;

ival
	: CONSTANT
		{ B_eval_ival($1, NULL); }
	| NAME
		{ B_eval_ival(0, $1); }
	;

function
	: NAME
		{ B_eval_function_def($1); B_control_function_start($1); }
	  LPAREN param_list_opt 
		{ B_eval_param_list(); }
	  RPAREN statement
		{ B_control_function_stop(); }
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
		{ B_eval_param_decl($1); }
	;

compound_statement
	: LBRACE RBRACE
	| LBRACE
		{ B_scope_enter(); }
	  statement_list
		{ B_scope_leave(); }
	  RBRACE
	;

statement_list
	: statement_list statement_cleanup
	| statement_cleanup
	;

statement_cleanup
	: statement
		{ EA_expr_cleanup(); }
	;

statement
	: AUTO auto_decl_list SEMI
		{ B_eval_auto_list(); }
	| EXTERN extrn_decl_list SEMI
	| expr SEMI
	| if_statement
	| WHILE
		{ B_control_while_start(); }
      LPAREN expr RPAREN
		{ B_control_while_test($4); }
      statement
		{ B_control_while_stop(); }
	| switch_statement
	| label_statement
	| BREAK SEMI
	| GOTO NAME SEMI
	| RETURN LPAREN expr RPAREN
		{ B_control_return($3); }
	  SEMI
	| RETURN
		{ B_control_return(-1); }
	  SEMI
	| SEMI
	| compound_statement
	| F_LCHAR LPAREN expr COMMA expr COMMA expr RPAREN
		{ B_builtin_lchar($3, $5, $7); }
	;

auto_decl_list
	: auto_decl_list COMMA auto_decl
	| auto_decl
	;

auto_decl
	: NAME
		{ B_eval_auto_decl($1, 0); }
	| NAME CONSTANT
		{ B_eval_auto_decl($1, $2); }
	;

extrn_decl_list
	: extrn_decl_list COMMA extrn_decl
	| extrn_decl
	;

extrn_decl
	: NAME
		{ B_eval_extern_decl($1); }
	;

if_statement
	: if_start statement %prec LOWER_THAN_ELSE
		{ B_control_if_stop(false); }
	| if_start statement ELSE
		{ B_control_if_stop(true); }
	  statement
		{ B_control_else_stop(); }
	;

if_start
	: IF LPAREN expr RPAREN
		{ B_control_if_start($3); }
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
		{ B_eval_assignment(BOP_NONE, $1, $3); }
    | expr_assignment ASSIGN_PLUS	expr_assignment
		{ B_eval_assignment(BOP_PLUS, $1, $3); }
    | expr_assignment ASSIGN_MINUS	expr_assignment
		{ B_eval_assignment(BOP_MINUS, $1, $3); }
    | expr_assignment ASSIGN_MULT	expr_assignment
		{ B_eval_assignment(BOP_MULT, $1, $3); }
    | expr_assignment ASSIGN_DIV	expr_assignment
		{ B_eval_assignment(BOP_DIV, $1, $3); }
    | expr_assignment ASSIGN_MOD	expr_assignment
		{ B_eval_assignment(BOP_MOD, $1, $3); }
    | expr_assignment ASSIGN_AND	expr_assignment
		{ B_eval_assignment(BOP_AND, $1, $3); }
    | expr_assignment ASSIGN_OR		expr_assignment
		{ B_eval_assignment(BOP_OR, $1, $3); }
    | expr_assignment ASSIGN_LSHIFT expr_assignment
		{ B_eval_assignment(BOP_SHL, $1, $3); }
    | expr_assignment ASSIGN_RSHIFT expr_assignment
		{ B_eval_assignment(BOP_SHR, $1, $3); }
    | expr_assignment ASSIGN_EQ 	expr_assignment
		{ B_eval_assignment(BOP_EQ, $1, $3); }
    | expr_assignment ASSIGN_NE 	expr_assignment
		{ B_eval_assignment(BOP_NE, $1, $3); }
    | expr_assignment ASSIGN_LT 	expr_assignment
		{ B_eval_assignment(BOP_LT, $1, $3); }
    | expr_assignment ASSIGN_GT 	expr_assignment
		{ B_eval_assignment(BOP_GT, $1, $3); }
    | expr_assignment ASSIGN_LE 	expr_assignment
		{ B_eval_assignment(BOP_LE, $1, $3); }
    | expr_assignment ASSIGN_GE 	expr_assignment
		{ B_eval_assignment(BOP_GE, $1, $3); }
    ;

expr_conditional
	: expr_logical_or
	| expr_logical_or QUESTION expr COLON expr_conditional
		{ $$ = B_compute_ternary($1, $3, $5); }
	;

expr_logical_or
	: expr_logical_and
	| expr_logical_or OR expr_logical_and
		{ $$ = B_compute_binary_op(BOP_OR, $1, $3); }
	;

expr_logical_and
	: expr_equality
	| expr_logical_and AND expr_equality
		{ $$ = B_compute_binary_op(BOP_AND, $1, $3); }
	;

expr_equality
	: expr_relational
	| expr_equality EQ expr_relational
		{ $$ = B_compute_comparison(BOP_EQ, $1, $3); }
	| expr_equality NE expr_relational
		{ $$ = B_compute_comparison(BOP_NE, $1, $3); }
	;

expr_relational
	: expr_shift
	| expr_relational LT expr_shift
		{ $$ = B_compute_comparison(BOP_LT, $1, $3); }
	| expr_relational LE expr_shift
		{ $$ = B_compute_comparison(BOP_LE, $1, $3); }
	| expr_relational GT expr_shift
		{ $$ = B_compute_comparison(BOP_GT, $1, $3); }
	| expr_relational GE expr_shift
		{ $$ = B_compute_comparison(BOP_GE, $1, $3); }
	;

expr_shift
	: expr_additive
	| expr_shift LSHIFT expr_additive
		{ $$ = B_compute_binary_op(BOP_SHL, $1, $3); }
	| expr_shift RSHIFT expr_additive
		{ $$ = B_compute_binary_op(BOP_SHR, $1, $3); }
	;

expr_additive
	: expr_multiplicative
	| expr_additive PLUS expr_multiplicative
		{ $$ = B_compute_binary_op(BOP_PLUS, $1, $3); }
	| expr_additive MINUS expr_multiplicative
		{ $$ = B_compute_binary_op(BOP_MINUS, $1, $3); }
	;

expr_multiplicative
	: expr_unary
	| expr_multiplicative MULT expr_unary
		{ $$ = B_compute_binary_op(BOP_MULT, $1, $3); }
	| expr_multiplicative DIV expr_unary
		{ $$ = B_compute_binary_op(BOP_DIV, $1, $3); }
	| expr_multiplicative MOD expr_unary
		{ $$ = B_compute_binary_op(BOP_MOD, $1, $3); }
	;

expr_unary
	: expr_postfix
	| MULT expr_unary %prec USTAR
		{ $$ = B_compute_unary_op(UOP_STAR, $2); }
	| AND expr_postfix %prec UAMP
		{ $$ = B_compute_unary_op(UOP_ADDR, $2); }
	| MINUS expr_unary %prec UMINUS
		{ $$ = B_compute_unary_op(UOP_NEG, $2); }
	| NOT expr_unary %prec UNOT
		{ $$ = B_compute_unary_op(UOP_NOT, $2); }
	| INCR expr_unary
		{ $$ = B_compute_unary_op(UOP_INCR, $2); }
	| DECR expr_unary
		{ $$ = B_compute_unary_op(UOP_DECR, $2); }
	;

expr_postfix
	: expr_primary
	| expr_builtin_val
	| expr_postfix INCR
	| expr_postfix DECR
	| expr_postfix LBRACKET expr RBRACKET
	| expr_postfix call_start LPAREN argument_list RPAREN
		{ $$ = B_eval_function_call($1); }
	| expr_postfix call_start LPAREN RPAREN
		{ $$ = B_eval_function_call($1); }
	;

call_start
	: { B_eval_function_call_start(); }
	;

expr_builtin_val
	: F_CHAR LPAREN expr COMMA expr RPAREN
		{ $$ = B_builtin_char($3, $5); }
	;

expr_primary
	: NAME
		{ $$ = B_eval_identifier($1); }
	| LPAREN expr RPAREN
		{ $$ = $2; }
	| constant
	;

argument_list
	: argument COMMA argument_list
	| argument
	;

argument
	: expr
		{ B_eval_function_arg($1); }
	;

constant
	: CONSTANT
		{ $$ = B_eval_constant($1, NULL); }
	| STR_CONSTANT
		{ $$ = B_eval_constant(0, $1); }
	| CHAR_CONSTANT
		{ $$ = B_eval_constant(0, $1); }
	;

%%
