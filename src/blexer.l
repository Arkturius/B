%{
	#include "bparser.h"
	#include <stdlib.h>
	#include <string.h>
%}

DIGIT       [0-9]
ALPHA       [a-zA-Z_]
CHAR        [^\n'\\]|\\[\\'\"nrtbf0]

%option noyywrap
%option noinput
%option nounput

%%

"auto"		return AUTO;
"extrn"		return EXTERN;
"if"		return IF;
"else"		return ELSE;
"switch"	return SWITCH;
"case"		return CASE;
"while"		return WHILE;
"return"	return RETURN;
"goto"		return GOTO;

"char"		return F_CHAR;
"lchar"		return F_LCHAR;

"==="		return ASSIGN_EQ;
"=!="		return ASSIGN_NE;
"=<="		return ASSIGN_LE;
"=>="		return ASSIGN_GE;
"=<<"		return ASSIGN_LSHIFT;
"=>>"		return ASSIGN_RSHIFT;
"=<"		return ASSIGN_LT;
"=>"		return ASSIGN_GT;
"=|"		return ASSIGN_OR;
"=&"		return ASSIGN_AND;
"=+"		return ASSIGN_PLUS;
"=-"		return ASSIGN_MINUS;
"=*"		return ASSIGN_MULT;
"=/"		return ASSIGN_DIV;
"=%"		return ASSIGN_MOD;

"=="		return EQ;
"!="		return NE;
"<="		return LE;
">="		return GE;
"<<"		return LSHIFT;
">>"		return RSHIFT;

"="			return ASSIGN;
"!"			return NOT;
"&"			return AND;
"|"			return OR;

"<"			return LT;
">"			return GT;

"+"			return PLUS;
"-"			return MINUS;
"*"			return MULT;
"/"			return DIV;
"%"			return MOD;

"?"			return QUESTION;
":"			return COLON;

"("			return LPAREN;
")"			return RPAREN;
"{"			return LBRACE;
"}"			return RBRACE;
"["			return LBRACKET;
"]"			return RBRACKET;

";"			return SEMI;
","			return COMMA;

-?0?{DIGIT}+				{ yylval.n = strtol(yytext, NULL, 0); return CONSTANT; }

'({CHAR}|{CHAR}{CHAR})'		{ yylval.s = strdup(yytext); return CHAR_CONSTANT; }

\"({CHAR}*)\"				{ yylval.s = strdup(yytext); return STR_CONSTANT; }

{ALPHA}({ALPHA}|{DIGIT})*	{ yylval.s = strdup(yytext); return NAME; }

[ \t]+						// Ignore whitespaces.
\n							{ yylineno++; }

"/\*"([^*]|\*+[^*/])*\*+"/"	// Ignore comments.

.							{ return yytext[0]; } // Character fallback.

%%
