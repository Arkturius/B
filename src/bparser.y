%{
	# include <stdio.h>
	# include <stdlib.h>
	# include <string.h>

	extern int yylex();
	extern int yylex_destroy(void);

	extern int yyparse();
	extern FILE *yyin;

	int yyerror(const char *s);

	#define BCOMPILER_IMPLEMENTATION
	#include <bcompiler.h>
%}

%code requires
{
	#include <types.h>
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
		{ B_function($1); }
	  LPAREN param_list_opt RPAREN compound_statement
		{ B_function_end($1); }
	;

param_list_opt
	: param_list
	| 
	;

param_list
	: param
		{ B_function_param($1); }
	| param_list COMMA param
		{ B_function_param($3); }
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
        { B_while_start(); }
      LPAREN expr RPAREN 
        { B_while($4); }
      statement
        { B_while_end(); }
	| RETURN LPAREN expr RPAREN SEMI
		{ B_expr_return($3); }
	| RETURN SEMI
		{ B_return(); }
	| SEMI
	| compound_statement
	;

auto_decl_list
	: auto_decl_list COMMA auto_decl
        { B_auto_decl();}
	| auto_decl
	;

auto_decl
	: NAME
        { B_auto_var($1); }
	;

extrn_decl_list
	: extrn_decl_list COMMA extrn_decl
	| extrn_decl
	;

extrn_decl
	: NAME
	;

if_statement
	: IF LPAREN expr RPAREN statement 
		{ B_if_block($3); B_if_end(); } %prec LOWER_THAN_ELSE
	| IF LPAREN expr RPAREN statement ELSE 
		{ B_if_else_block($3); }
	  statement
		{ B_if_end(); }
	;

expr
	: expr_assignment	
	;

expr_assignment
	: expr_conditional
	| expr_assignment ASSIGN expr_assignment
		{ $$ = B_assign(ASSIGN, $1, $3); }
    | expr_assignment ASSIGN_PLUS expr_assignment
		{ $$ = B_assign(ASSIGN_PLUS, $1, $3); }
    | expr_assignment ASSIGN_MINUS expr_assignment
		{ $$ = B_assign(ASSIGN_MINUS, $1, $3); }
    | expr_assignment ASSIGN_MULT expr_assignment
		{ $$ = B_assign(ASSIGN_MULT, $1, $3); }
    | expr_assignment ASSIGN_DIV expr_assignment
		{ $$ = B_assign(ASSIGN_DIV, $1, $3); }
    | expr_assignment ASSIGN_MOD expr_assignment
		{ $$ = B_assign(ASSIGN_MOD, $1, $3); }
    | expr_assignment ASSIGN_AND expr_assignment
		{ $$ = B_assign(ASSIGN_AND, $1, $3); }
    | expr_assignment ASSIGN_OR expr_assignment
		{ $$ = B_assign(ASSIGN_OR, $1, $3); }
    | expr_assignment ASSIGN_LT expr_assignment
		{ $$ = B_assign(ASSIGN_LT, $1, $3); }
    | expr_assignment ASSIGN_GT expr_assignment
		{ $$ = B_assign(ASSIGN_GT, $1, $3); }
    | expr_assignment ASSIGN_LE expr_assignment
		{ $$ = B_assign(ASSIGN_LE, $1, $3); }
    | expr_assignment ASSIGN_GE expr_assignment
		{ $$ = B_assign(ASSIGN_GE, $1, $3); }
    | expr_assignment ASSIGN_EQ expr_assignment
		{ $$ = B_assign(ASSIGN_EQ, $1, $3); }
    | expr_assignment ASSIGN_NE expr_assignment
		{ $$ = B_assign(ASSIGN_NE, $1, $3); }
    | expr_assignment ASSIGN_LSHIFT expr_assignment
		{ $$ = B_assign(ASSIGN_LSHIFT, $1, $3); }
    | expr_assignment ASSIGN_RSHIFT expr_assignment
		{ $$ = B_assign(ASSIGN_RSHIFT, $1, $3); }
    ;

expr_conditional
	: expr_logical_or
	| expr_logical_or QUESTION expr COLON expr_conditional
		{ $$ = B_ternary($1, $3, $5); }
	;

expr_logical_or
	: expr_logical_xor
	| expr_logical_or OR expr_logical_xor
		{ $$ = B_binary_op(B_OP_OR, $1, $3); }
	;

expr_logical_xor
	: expr_logical_and
	| expr_logical_xor XOR expr_logical_and
		{ $$ = B_binary_op(B_OP_XOR, $1, $3); }
	;

expr_logical_and
	: expr_equality
	| expr_logical_and AND expr_equality
		{ $$ = B_binary_op(B_OP_AND, $1, $3); }
	;

expr_equality
	: expr_relational
	| expr_equality EQ expr_relational
		{ $$ = B_comp_equal($1, $3); }
	| expr_equality NE expr_relational
		{ $$ = B_comp_not_equal($1, $3); }
	;

expr_relational
	: expr_shift
	| expr_relational LT expr_shift
		{ $$ = B_comp_lower_than($1, $3); }
	| expr_relational LE expr_shift
		{ $$ = B_comp_lower_equal($1, $3); }
	| expr_relational GT expr_shift
		{ $$ = B_comp_greater_than($1, $3); }
	| expr_relational GE expr_shift
		{ $$ = B_comp_greater_equal($1, $3); }
	;

expr_shift
	: expr_additive
	| expr_shift LSHIFT expr_additive
		{ $$ = B_binary_op(B_OP_SHL, $1, $3); }
	| expr_shift RSHIFT expr_additive
		{ $$ = B_binary_op(B_OP_SHR, $1, $3); }
	;

expr_additive
	: expr_multiplicative
	| expr_additive PLUS expr_multiplicative
		{ $$ = B_binary_op(B_OP_ADD, $1, $3); }
	| expr_additive MINUS expr_multiplicative
		{ $$ = B_binary_op(B_OP_SUB, $1, $3); }
	;

expr_multiplicative
	: expr_unary
	| expr_multiplicative MULT expr_unary
		{ $$ = B_binary_op(B_OP_MUL, $1, $3); }
	| expr_multiplicative DIV expr_unary
		{ $$ = B_binary_op(B_OP_DIV, $1, $3); }
	| expr_multiplicative MOD expr_unary
		{ $$ = B_binary_op(B_OP_MOD, $1, $3); }
	;

expr_unary
	: expr_postfix
	| MULT expr_unary %prec USTAR
		{ $$ = B_unary_deref($2); }
	| AND expr_postfix %prec UAMP
		{ $$ = B_unary_addrof($2); }
	| MINUS expr_unary %prec UMINUS
		{ $$ = B_unary_negate($2); }
	| NOT expr_unary %prec UNOT
		{ $$ = B_unary_invert($2); }
	| INCR expr_unary
		{ $$ = B_unary_pre_incr($2); }
	| DECR expr_unary
		{ $$ = B_unary_pre_decr($2); }
	;

expr_postfix
	: expr_primary
	| expr_builtin
	| expr_postfix INCR
		{ $$ = B_unary_incr($1); }
	| expr_postfix DECR
		{ $$ = B_unary_decr($1); }
	| expr_postfix LBRACKET expr RBRACKET
		{ $$ = B_binary_op(B_OP_SUBSCRIPT, $1, $3); }
	| expr_postfix LPAREN argument_list RPAREN
		{ $$ = B_function_call($1); }
	| expr_postfix LPAREN RPAREN
		{ $$ = B_function_call($1); }
	;

expr_builtin
	: F_CHAR LPAREN expr COMMA expr RPAREN
		{ $$ = B_builtin_char($3, $5); }
	| F_LCHAR LPAREN expr COMMA expr COMMA expr RPAREN
		{ $$ = B_builtin_lchar($3, $5, $7); }
	;

expr_primary
	: NAME
		{ $$ = B_expr_variable($1); }
	| LPAREN expr RPAREN
		{ $$ = $2; }
	| constant
	;

argument_list
	: expr
		{ B_function_arg($1); }
	| argument_list COMMA expr
		{ B_function_arg($3); }
	;

constant
	:CONSTANT
		{ $$ = B_expr_constant($1); }
	|STR_CONSTANT
		{ $$ = B_expr_string($1); }
	|CHAR_CONSTANT
		{ $$ = B_expr_char($1); }
	;

%%

#include <unistd.h>

int main(int argc, char **argv)
{
	B_compiler_start(&bcp);

	if (argc > 1)
	{
		yyin = fopen(argv[1], "r");
		if (!yyin)
		{
			perror(argv[1]);
			return (1);
		}
	}
	int result = yyparse();
	
	fclose(yyin);
	yylex_destroy();
	B_compiler_stop(&bcp);

	return result;
}

int	yyerror(const char *s)
{
    fprintf(stderr, "> parse error: %s\n", s);
    return (1);
}
