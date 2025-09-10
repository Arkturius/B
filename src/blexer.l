%{
	#include "bparser.h"
	#include <stdlib.h>
	#include <string.h>
%}

DIGIT       [0-9]
ALPHA       [a-zA-Z_]
CHAR        [^\n'\\]|\\[\\'\"nt0*)(]

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
"default"	return DEFAULT;
"while"		return WHILE;
"return"	return RETURN;
"goto"		return GOTO;
"break"		return BREAK;

"char"		return F_CHAR;
"lchar"		return F_LCHAR;

"==="		{ yylval.n = ASSIGN_EQ; return ASSIGN_EQ; }
"=!="		{ yylval.n = ASSIGN_NE; return ASSIGN_NE; }
"=<="		{ yylval.n = ASSIGN_LE; return ASSIGN_LE; }
"=>="		{ yylval.n = ASSIGN_GE; return ASSIGN_GE; }
"=<<"		{ yylval.n = ASSIGN_LSHIFT; return ASSIGN_LSHIFT; }
"=>>"		{ yylval.n = ASSIGN_RSHIFT; return ASSIGN_RSHIFT; }
"=<"		{ yylval.n = ASSIGN_LT; return ASSIGN_LT; }
"=>"		{ yylval.n = ASSIGN_GT; return ASSIGN_GT; }
"=|"		{ yylval.n = ASSIGN_OR; return ASSIGN_OR; }
"=&"		{ yylval.n = ASSIGN_AND; return ASSIGN_AND; }
"=+"		{ yylval.n = ASSIGN_PLUS; return ASSIGN_PLUS; }
"=-"		{ yylval.n = ASSIGN_MINUS; return ASSIGN_MINUS; }
"=*"		{ yylval.n = ASSIGN_MULT; return ASSIGN_MULT; }
"=/"		{ yylval.n = ASSIGN_DIV; return ASSIGN_DIV; }
"=%"		{ yylval.n = ASSIGN_MOD; return ASSIGN_MOD; }

"=="		return EQ;
"!="		return NE;
"<="		return LE;
">="		return GE;
"<<"		return LSHIFT;
">>"		return RSHIFT;
"++"		return INCR;
"--"		return DECR;

"="			return ASSIGN;
"!"			return NOT;
"&"			return AND;
"|"			return OR;
"^"			return XOR;

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
