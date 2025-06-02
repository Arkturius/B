%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdint.h>
	#include <string.h>

	#define	WORD	4

	extern int yylex();
	extern int yyparse();
	extern FILE *yyin;
	int yyerror(const char *s);

%}

%union {
	int		n;
	char	*s;
}

%token<n>	CONSTANT
%token<s>	CHAR_CONSTANT
%token<s>	STR_CONSTANT
%token<s>	NAME

%token		AUTO EXTERN RETURN GOTO
%token		IF ELSE WHILE SWITCH CASE
%token		ASSIGN SEMI COMMA COLON
%token		EQ_ASSIGN EQ_PLUS EQ_MINUS EQ_STAR EQ_SLASH EQ_PERCENT
%token		EQ_NE EQ_LT EQ_LE EQ_GT EQ_GE EQ_RSHIFT EQ_LSHIFT EQ_AND EQ_OR
%token		LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET
%token		INCR DECR
%token		PLUS MINUS STAR SLASH PERCENT
%token		EQ NE LT LE GT GE
%token		LSHIFT RSHIFT
%token		AND OR NOT
%token		QUESTION

%type<n>	constant

%left		LBRACKET LPAREN
%right		ASSIGN 
%right		EQ_ASSIGN EQ_PLUS EQ_MINUS EQ_STAR EQ_SLASH EQ_PERCENT
%right		EQ_NE EQ_LT EQ_LE EQ_GT EQ_GE EQ_RSHIFT EQ_LSHIFT EQ_AND EQ_OR
%left		OR
%left		AND
%left		EQ NE
%left		LT LE GT GE
%left		LSHIFT RSHIFT
%left		PLUS MINUS
%left		STAR SLASH PERCENT
%right		UNARY
%nonassoc	INCR DECR
%right		QUESTION COLON
%right		ELSE

%%

program:		/* EMPTY */
				|program definition
				;

definition:		NAME array init_lst SEMI
				{
					printf("ARRAY DEF\n");
				}
				|NAME
				{
					printf("FUNCTION DEF");
				}
				LPAREN param_lst RPAREN statement
				;

array:			/* EMPTY */
				|LBRACKET constant_opt RBRACKET
				;

constant_opt:	/* EMPTY */
				|constant
				;

init_lst:		/* EMPTY */
				|ival_lst
				;

ival_lst:		ival
				|ival_lst COMMA ival
				;

ival:			constant
				|NAME
				;

param_lst:		/* EMPTY */
				|name_lst
				;

name_lst:		NAME
				|name_lst COMMA NAME
				;

statement:		AUTO
				{
					printf("AUTO declaration:\n");
				}
				auto_decl_lst SEMI statement
				|EXTERN
				{
					printf("EXTERN declaration:\n");
				}
				name_lst SEMI statement
				|NAME COLON statement
				{
					printf("LABEL [%s]\n", $1);
				}
				|CASE constant COLON statement
				{
					printf("CASE [%d]\n", $2);
				}
				|LBRACE
				{
					printf("SCOPE_OPEN\n");
				}
				statement_lst
				{
					printf("SCOPE_CLOSE\n");
				}
				RBRACE
				|IF LPAREN 
				{
					printf("IF BLOCK\n");
				}
				rvalue RPAREN statement else_opt
				|WHILE LPAREN rvalue RPAREN statement
				{
					printf("WHILE BLOCK\n");
				}
				|SWITCH rvalue statement
				{
					printf("SWITCH BLOCK\n");
				}
				|GOTO rvalue SEMI
				{
					printf("GOTO\n");
				}
				|RETURN return_expr SEMI
				{
					printf("RETURN\n");
				}
				|rvalue_opt SEMI
				{
					printf("EXPRESSION\n");
				}
				;

auto_decl_lst:	auto_decl
				|auto_decl_lst COMMA auto_decl
				;

auto_decl:		NAME constant_opt
				{
					printf("  AUTOVAR [%s]\n", $1);
				}
				;

statement_lst:	/* EMPTY */
				|statement_lst statement
				;

else_opt:		/* EMPTY */
				|ELSE statement
				;

return_expr:	/* EMPTY */
				|LPAREN rvalue RPAREN

rvalue_opt:		/* EMPTY */
				| rvalue
				;

rvalue:			LPAREN rvalue RPAREN
				|constant
				|lvalue
				|lvalue assign rvalue
				{
					printf("ASSIGNMENT\n");
				}
				|inc_dec lvalue
				{
					printf("PRE INCR/DECR\n");
				}
				|lvalue inc_dec %prec INCR
				{
					printf("POST INCR/DECR\n");
				}
				|unary lvalue %prec UNARY
				{
					printf("-/! OPERATOR\n");
				}
				|AND lvalue
				{
					printf("ADDRESS OF\n");
				}
				|rvalue binary rvalue
				{
					printf("BINARY OP\n");
				}
				|rvalue QUESTION rvalue COLON rvalue
				{
					printf("TERNARY OP\n");
				}
				|rvalue LPAREN rvalue_lst RPAREN
				{
					printf("FUNCALL\n");
				}
				;

rvalue_lst:		/* EMPTY */
				|rvalue
				|rvalue_lst COMMA rvalue
				;

assign:			ASSIGN
				|EQ_ASSIGN
				|EQ_PLUS
				|EQ_MINUS
				|EQ_STAR
				|EQ_SLASH
				|EQ_PERCENT
				|EQ_NE
				|EQ_LT
				|EQ_LE
				|EQ_GT
				|EQ_GE
				|EQ_RSHIFT
				|EQ_LSHIFT
				|EQ_AND
				|EQ_OR
				;

inc_dec:		INCR
				|DECR
				;

unary:			MINUS
				|NOT

binary:			EQ | NE | LT | LE | GT | GE
				|AND
				|OR
				|LSHIFT
				|RSHIFT
				|PLUS
				|MINUS
				|STAR
				|SLASH
				|PERCENT
				;

lvalue:			NAME
				{
					printf("IDENTIFIER [%s]\n", $1);
				}
				|STAR rvalue %prec UNARY
				{
					printf("DEREFERENCE\n");
				}
				|rvalue LBRACKET rvalue RBRACKET
				{
					printf("ARRAY ACCESS\n");
				}
				;

constant:		CONSTANT
				{
					printf("NUMERIC CONSTANT [%d]\n", $1);
					$$ = $1;
				}
				|CHAR_CONSTANT
				{
					printf("CHAR CONSTANT [%s]\n", $1);
					if (strlen($1) == 3)
						$$ = $1[1];
					else if (strlen($1) == 4 && $1[1] == '\\')
					{
						switch($1[2])
						{
							case 'n': $$ = '\n'; break;
							case 't': $$ = '\t'; break;
							case 'r': $$ = '\r'; break;
							case 'b': $$ = '\b'; break;
							case 'f': $$ = '\f'; break;
							case '0': $$ = '\0'; break;
							case '\\': $$ = '\\'; break;
							case '\'': $$ = '\''; break;
							case '\"': $$ = '\"'; break;
							default: $$ = $1[2]; break;
						}
					}
					else
						$$ = 0;
				}
				|STR_CONSTANT
				{
					printf("STRING CONSTANT [%s]\n", $1);
					$$ = 0;
				}
				;

%%

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
	return yyparse();
}

int	yyerror(const char *s)
{
    fprintf(stderr, "Error: %s\n", s);
    return (1);
}
