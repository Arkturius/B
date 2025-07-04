%{
	# include <stdio.h>
	# include <stdlib.h>
	# include <string.h>

	extern int yylex();
	extern int yylex_destroy(void);

	extern int yyparse();
	extern FILE *yyin;

	int yyerror(const char *s);

	# define BCOMPILER_IMPL
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
	Expression	e;
}

%token<n>	CONSTANT
%token<s>	CHAR_CONSTANT
%token<s>	STR_CONSTANT
%token<s>	NAME

%token		AUTO EXTERN WHILE RETURN IF ELSE SWITCH CASE GOTO
%token		F_CHAR F_LCHAR
%token		SEMI COMMA
%token		LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET
%token		QUESTION COLON

%token		ASSIGN
%token		ASSIGN_EQ ASSIGN_NE ASSIGN_LE ASSIGN_GE ASSIGN_LT ASSIGN_GT
%token		ASSIGN_LSHIFT ASSIGN_RSHIFT
%token		ASSIGN_OR ASSIGN_AND
%token		ASSIGN_PLUS ASSIGN_MINUS ASSIGN_MULT ASSIGN_DIV ASSIGN_MOD
%token		OR
%token		AND NOT
%token		EQ NE
%token		LT LE GT GE
%token		LSHIFT RSHIFT
%token		PLUS MINUS
%token		MULT DIV MOD

%right		ASSIGN
%right		ASSIGN_EQ ASSIGN_NE ASSIGN_LE ASSIGN_GE ASSIGN_LT ASSIGN_GT
%right		ASSIGN_LSHIFT ASSIGN_RSHIFT
%right		ASSIGN_OR ASSIGN_AND
%right		ASSIGN_PLUS ASSIGN_MINUS ASSIGN_MULT ASSIGN_DIV ASSIGN_MOD
%left		OR
%left		AND NOT
%left		EQ NE
%left		LT LE GT GE
%left		LSHIFT RSHIFT
%left		PLUS MINUS
%left		MULT DIV MOD

%right		UMINUS UAMP USTAR UNOT
%left		POSTFIX

%nonassoc	LOWER_THAN_ELSE
%nonassoc	ELSE

%type<e>	expr
%type<e>	lvalue
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
%type<e>	expr_builtin
%type<e>	expr_primary
%type<e>	argument_list
%type<e>	constant

%%

program
	: { b_program(); }
		definition_list
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
		{ b_function_start($1); }
		LPAREN param_list_opt RPAREN compound_statement
		{ b_function_end(); }
	;

param_list_opt
	: param_list
	| 
	;

param_list
	: param_list COMMA param
	| param
	;

param
	: NAME
	;

compound_statement
	: LBRACE statement_list RBRACE
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
	| WHILE LPAREN expr RPAREN statement
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

lvalue
	: NAME
		{ $$ = b_expr_autovar($1); }
	| MULT expr_unary %prec USTAR
		{ $$ = $2; }
	| expr_postfix LBRACKET expr RBRACKET
		{ $$ = b_expr_array($1, $3); }
	;

expr_assignment
	: expr_conditional
	| lvalue ASSIGN expr_assignment
    | lvalue ASSIGN_PLUS expr_assignment
    | lvalue ASSIGN_MINUS expr_assignment
    | lvalue ASSIGN_MULT expr_assignment
    | lvalue ASSIGN_DIV expr_assignment
    | lvalue ASSIGN_MOD expr_assignment
    | lvalue ASSIGN_AND expr_assignment
    | lvalue ASSIGN_OR expr_assignment
    | lvalue ASSIGN_LT expr_assignment
    | lvalue ASSIGN_GT expr_assignment
    | lvalue ASSIGN_LE expr_assignment
    | lvalue ASSIGN_GE expr_assignment
    | lvalue ASSIGN_EQ expr_assignment
    | lvalue ASSIGN_NE expr_assignment
    | lvalue ASSIGN_LSHIFT expr_assignment
    | lvalue ASSIGN_RSHIFT expr_assignment
    ;

expr_conditional
	: expr_logical_or
	| expr_logical_or QUESTION expr COLON expr_conditional
	;

expr_logical_or
	: expr_logical_and
	| expr_logical_or OR expr_logical_and
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
	| AND expr_postfix %prec UAMP
		{ $$ = $2; }
	| MINUS expr_unary %prec UMINUS
		{ $$ = $2; }
	| NOT expr_unary %prec UNOT
		{ $$ = $2; }
	;

expr_postfix
	: expr_primary
	| expr_builtin
	| expr_postfix LPAREN argument_list RPAREN
	| expr_postfix LPAREN RPAREN
	;

expr_builtin
	: F_CHAR LPAREN expr COMMA expr RPAREN
		{ $$ = b_builtin_char($3, $5); }
	| F_LCHAR LPAREN expr COMMA expr COMMA expr RPAREN
		{ $$ = b_builtin_lchar($3, $5, $7); }
	;

expr_primary
	: NAME
		{ $$ = b_expr_autovar($1); }
	| constant
	| LPAREN expr RPAREN
		{ $$ = $2; }
	;

argument_list
	: expr
	| argument_list COMMA expr
	;

constant
	:CONSTANT
		{ $$ = b_expr_constant(CONSTANT_INT, $1, NULL); }
	|STR_CONSTANT
		{ $$ = b_expr_constant(CONSTANT_STR, 0, $1); }
	|CHAR_CONSTANT
		{ $$ = b_expr_constant(CONSTANT_CHR, 0, $1); }
	;

%%

#include <unistd.h>

int main(int argc, char **argv)
{
	if (argc > 1)
	{
		yyin = fopen(argv[1], "r");
		if (!yyin)
		{
			perror(argv[1]);
			return (1);
		}
	}

	BCompiler	compiler = {0};

	int result = yyparse();

	yylex_destroy();
	
	return result;
}

int	yyerror(const char *s)
{
    fprintf(stderr, "Parse error: %s\n", s);
    return (1);
}
