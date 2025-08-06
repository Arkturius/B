%{
	# include <stdio.h>
	# include <stdlib.h>
	# include <string.h>

	extern int yylex();
	extern int yylex_destroy(void);

	extern int yyparse();
	extern FILE *yyin;

	int yyerror(const char *s);

	#include <gascheck.h>
%}

%union
{
	const char	*s;
	int			n;
}

%token<s>	STR_LITERAL
%token<s>	REGISTER
%token<s>	IDENT
%token<n>	NUMBER

%token		DIR_SYNTAX DIR_SECTION DIR_GLOBAL DIR_LONG DIR_STRING DIR_SET
%token		SECTION_TEXT SECTION_RODATA
%token		OFFSET
%token<s>	MEMSIZE

%token		COLON COMMA
%token		LBRACK RBRACK LPAREN RPAREN PLUS MINUS STAR

%token		NEWLINE

%left		PLUS MINUS

%%

file
	: lines { printf("[DEBUG] - Parsed complete file\n"); }
	;

lines
	 : lines line { printf("--------------------------\n\n"); }
	 | line { printf("[DEBUG] - First line\n"); }
	 ;

line
	: directive {printf("before nl\n");} NEWLINE { printf("[DEBUG] - Parsed directive line\n"); }
	| label NEWLINE { printf("[DEBUG] - Parsed label line\n"); }
	| instruction NEWLINE { printf("[DEBUG] - Parsed instruction line\n"); }
	| NEWLINE { printf("[DEBUG] - Empty line\n"); }
	;

directive
	: DIR_SYNTAX { printf("[DEBUG] - Directive: .intel_syntax\n"); }
	| DIR_SECTION section { printf("[DEBUG] - Directive: .section\n"); }
	| DIR_GLOBAL IDENT { printf("[DEBUG] - Directive: .globl %s\n", $2); }
	| DIR_LONG STR_LITERAL longoff { printf("[DEBUG] - Directive: .long %s\n", $2); }
	| DIR_STRING STR_LITERAL { printf("[DEBUG] - Directive: .string %s\n", $2); }
	| DIR_SET IDENT COMMA expr
	;

longoff
	: PLUS NUMBER { printf("[DEBUG] - Long offset: +%d\n", $2); }
	| /* EMPTY */ { printf("[DEBUG] - No long offset\n"); }
	;

section
	: SECTION_TEXT { printf("[DEBUG] - Section: .text\n"); }
	| SECTION_RODATA { printf("[DEBUG] - Section: .rodata\n"); }
	;

label
	: IDENT COLON { printf("[DEBUG] - Label: %s:\n", $1); }
	;

instruction
	: IDENT { printf(" [DEBUG] - Instruction: %s (no operands)\n", $1); }
	| IDENT operand { printf(" [DEBUG] - Instruction: %s (1 operand)\n", $1); }
	| IDENT operand COMMA operand { printf(" [DEBUG] - Instruction: %s (2 operands)\n", $1); }
	;

operand
	: REGISTER { printf("  [DEBUG] - Operand: register %s\n", $1); }
	| mem_operand { printf("  [DEBUG] - Operand: memory operand\n"); }
	| expr { printf("  [DEBUG] - Operand: expression\n"); }
	;

mem_operand
	: LBRACK mem_expr RBRACK { printf("[DEBUG] - Memory operand: [expr]\n"); }
	| MEMSIZE LBRACK mem_expr RBRACK { printf("[DEBUG] - Memory operand: MEMSIZE [expr]\n"); }
	;

mem_expr
	: REGISTER { printf("   [DEBUG] - Memory expr: register %s\n", $1); }
	| REGISTER PLUS NUMBER { printf("   [DEBUG] - Memory expr: %s + %d\n", $1, $3); }
	| REGISTER MINUS NUMBER { printf("   [DEBUG] - Memory expr: %s - %d\n", $1, $3); }
	| NUMBER { printf("   [DEBUG] - Memory expr: number %d\n", $1); }
	| IDENT { printf("   [DEBUG] - Memory expr: identifier %s\n", $1); }
	;

expr
	: IDENT { printf("   [DEBUG] - Expression: identifier %s\n", $1); }
	| NUMBER { printf("   [DEBUG] - Expression: number %d\n", $1); }
	| expr PLUS expr { printf("   [DEBUG] - Expression: addition\n"); }
	| expr MINUS expr { printf("   [DEBUG] - Expression: subtraction\n"); }
	| OFFSET IDENT { printf("   [DEBUG] - Expression: OFFSET %s\n", $2); }
	| LPAREN expr RPAREN { printf("   [DEBUG] - Expression: parenthesized\n"); }
	;

%%

#include <unistd.h>
#define ARG_SHIFT(ac, av)	(ac--, *av++)

int main(int argc, char **argv)
{
	yyin = stdin;

	const char	*exe = ARG_SHIFT(argc, argv);

	if (argc > 0)
	{
		const char	*target = ARG_SHIFT(argc, argv);

		yyin = fopen(target, "r");
		if (!yyin)
		{
			perror(target);
			return (1);
		}
	}
	int	result = yyparse();

	yylex_destroy();
	return (result);
}

int	yyerror(const char *s)
{
    fprintf(stderr, "Parse error: %s\n", s);
    return (1);
}

/*
file
	: lines
	;

lines
	 : lines line
	 | line
	 ;

line
	: directive NEWLINE
	| label NEWLINE
	| instruction NEWLINE
	| NEWLINE
	;

directive
	: DIR_SYNTAX section
	| DIR_SECTION section
	| DIR_GLOBAL IDENT
	| DIR_LONG STR_LITERAL longoff
	| DIR_STRING STR_LITERAL
	;

longoff
	: PLUS NUMBER
	|  EMPTY 
	;

section
	: SECTION_TEXT
	| SECTION_RODATA
	;

label
	: IDENT COLON
	;

instruction
	: IDENT
	| IDENT operand
	| IDENT operand COMMA operand
	;

operand
	: REGISTER
	| NUMBER
	| IDENT
	| mem_operand
	| LBRACK IDENT RBRACK
	| expr
	;

mem_operand
	: LBRACK mem_expr RBRACK
	| MEMSIZE LBRACK mem_expr RBRACK
	;

mem_expr
	: REGISTER
	| REGISTER PLUS NUMBER
	| REGISTER MINUS NUMBER
	| NUMBER
	| IDENT
	;

expr
	: IDENT
	| NUMBER
	| expr PLUS expr
	| expr MINUS expr
	| OFFSET IDENT
	| LPAREN expr RPAREN
	;
*/
