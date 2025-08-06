%{
	#include "gasparser.h"
	#include <stdlib.h>
	#include <string.h>
%}

DIGIT       [0-9]
ALPHA       [a-zA-Z_]
CHAR        [^\n'\\]|\\[\\'\"nrtbf0]
REGISTER	(e?[abcd]x|[abcd][hl]|e?[sd]i|e?[sb]p)

%option noyywrap
%option noinput
%option nounput

%%

".intel_syntax noprefix"	return DIR_SYNTAX;
".section"					return DIR_SECTION;
".globl"					return DIR_GLOBAL;
".long"						return DIR_LONG;
".string"					return DIR_STRING;
".set"						return DIR_SET;

".text"						return SECTION_TEXT;
".rodata"					return SECTION_RODATA;

("DWORD"|"WORD"|"BYTE")[ \t]+"PTR" { yylval.s = strdup(yytext); return MEMSIZE; }

"OFFSET"					return OFFSET;

{REGISTER}					{ yylval.s = strdup(yytext); return REGISTER; }

[a-zA-Z_.][a-zA-Z0-9_@.]*   { yylval.s = strdup(yytext); return IDENT; }

":"                         { return COLON; }
","                         { return COMMA; }

"0x"[0-9a-fA-F]+            { yylval.n = strtol(yytext, NULL, 0); return NUMBER; }
[0-9]+                      { yylval.n = strtol(yytext, NULL, 10); return NUMBER; }

"["                         { return LBRACK; }
"]"                         { return RBRACK; }
"("                         { return LPAREN; }
")"                         { return RPAREN; }
"+"                         { return PLUS; }
"-"                         { return MINUS; }
"*"                         { return STAR; }

\"([^"\\]|\\.)*\"           { yylval.s = strdup(yytext); return STR_LITERAL; }

[ \t\r]+                    { /* ignore whitespaces */ }
\n                          { return NEWLINE; }

.                           { return yytext[0]; }

%%
