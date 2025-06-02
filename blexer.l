%{
	#include "bparser.h"
	#include <stdlib.h>
	#include <string.h>
%}

DIGIT       [0-9]
ALPHA       [a-zA-Z_]
CHAR        [^\n'\\]|\\[\\'\"nrtbf0]

%option noyywrap

%%

"auto"		return AUTO;
"extrn"		return EXTERN;
"return"	return RETURN;
"goto"		return GOTO;

"if"		return IF;
"else"		return ELSE;
"while"		return WHILE;
"switch"	return SWITCH;
"case"		return CASE;

"==="		return EQ_ASSIGN;
"=="		return EQ;
"="			return ASSIGN;

"=+"		return EQ_PLUS;
"=-"		return EQ_MINUS;
"=*"		return EQ_STAR;
"=/"		return EQ_SLASH;
"=%"		return EQ_PERCENT;
"=!="		return EQ_NE;
"=<"		return EQ_LT;
"=<="		return EQ_LE;
"=>"		return EQ_GT;
"=>="		return EQ_GE;
"=>>"		return EQ_RSHIFT;
"=<<"		return EQ_LSHIFT;
"=&"		return EQ_AND;
"=|"		return EQ_OR;    

"("			return LPAREN;
")"			return RPAREN;
"{"			return LBRACE;
"}"			return RBRACE;
"["			return LBRACKET;
"]"			return RBRACKET;

"++"		return INCR;
"--"		return DECR;

"+"			return PLUS;
"-"			return MINUS;
"*"			return STAR;
"/"			return SLASH;
"%"			return PERCENT;
"!="		return NE;
"<"			return LT;
"<="		return LE;
">"			return GT;
">="		return GE;
">>"		return RSHIFT;
"<<"		return LSHIFT;
"&"			return AND;
"|"			return OR;

"!"			return NOT;
"?"			return QUESTION;

0?{DIGIT}+					{ yylval.n = strtol(yytext, NULL, 0); return CONSTANT; }

'({CHAR}|{CHAR}{CHAR})'		{ yylval.s = strdup(yytext); return CHAR_CONSTANT; }

\"({CHAR}*)\"				{ yylval.s = strdup(yytext); return STR_CONSTANT; }

{ALPHA}({ALPHA}|{DIGIT})*	{ yylval.s = strdup(yytext); return NAME; }

[ \t\n]+
"/\*"([^*]|\*+[^*/])*\*+"/"
.                           { return yytext[0]; }

%%
