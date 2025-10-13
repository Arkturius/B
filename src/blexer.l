%{
	#include "bparser.h"
	#include <stdlib.h>
	#include <string.h>

	int			b_col = 0;
	const char	*b_row_start = NULL;
	char		b_hold_char;

	#define YY_USER_ACTION	{ b_col += yyleng; b_hold_char = yy_hold_char; }
%}

DIGIT       [0-9]
ALPHA       [a-zA-Z_]

CHAR        [^\n'\\]|\\[\\'\"nt0*)(]

STRING		\"([^\\\n]|(\\.))\"

ESCAPE			\*([0en\\t\*'\"])
CHAR_IN_STR		([^"\n]|{ESCAPE})
CHAR_IN_CHAR	([^'\n]|{ESCAPE})

%option noyywrap
%option noinput
%option nounput

%%

"auto"		{ return AUTO;			}
"extrn"		{ return EXTERN;  		}
"if"		{ return IF;      		}
"else"		{ return ELSE;    		}
"switch"	{ return SWITCH;  		}
"case"		{ return CASE;    		}
"default"	{ return DEFAULT; 		}
"while"		{ return WHILE;   		}
"return"	{ return RETURN;  		}
"goto"		{ return GOTO;    		}
"break"		{ return BREAK;   		}

"char"		{ return F_CHAR;  		}
"lchar"		{ return F_LCHAR; 		}

"==="		{ return ASSIGN_EQ;     }
"=!="		{ return ASSIGN_NE;     }
"=<="		{ return ASSIGN_LE;     }
"=>="		{ return ASSIGN_GE;     }
"=<<"		{ return ASSIGN_LSHIFT; }
"=>>"		{ return ASSIGN_RSHIFT; }
"=<"		{ return ASSIGN_LT;     }
"=>"		{ return ASSIGN_GT;     }
"=|"		{ return ASSIGN_OR;     }
"=&"		{ return ASSIGN_AND;    }
"=+"		{ return ASSIGN_PLUS;   }
"=-"		{ return ASSIGN_MINUS;  }
"=*"		{ return ASSIGN_MULT;   }
"=/"		{ return ASSIGN_DIV;    }
"=%"		{ return ASSIGN_MOD;    }

"=="		{ return EQ;			}
"!="		{ return NE; 			}
"<="		{ return LE; 			}
">="		{ return GE; 			}
"<<"		{ return LSHIFT;		}
">>"		{ return RSHIFT; 		}
"++"		{ return INCR;			}
"--"		{ return DECR; 			}

"="			{ return ASSIGN;		}
"!"			{ return NOT;			}
"&"			{ return AND;			}
"|"			{ return OR;			}
"^"			{ return XOR;			}

"<"			{ return LT;			}
">"			{ return GT;			}

"+"			{ return PLUS;			}
"-"			{ return MINUS;			}
"*"			{ return MULT;			}
"/"			{ return DIV;			}
"%"			{ return MOD;			}

"?"			{ return QUESTION;		}
":"			{ return COLON;			}

"("			{ return LPAREN;		}
")"			{ return RPAREN;		}
"{"			{ return LBRACE;		}
"}"			{ return RBRACE;		}
"["			{ return LBRACKET;		}
"]"			{ return RBRACKET;		}

";"			{ return SEMI;			}
","			{ return COMMA;			}

-?0[xX][0-9a-fA-F]+				{ yylval.n = strtol(yytext, NULL, 0); return CONSTANT; }
-?0?{DIGIT}+					{ yylval.n = strtol(yytext, NULL, 0); return CONSTANT; }

'{CHAR_IN_CHAR}'				{ yylval.s = strdup(yytext); return CHAR_CONSTANT; } 
'{CHAR_IN_CHAR}{CHAR_IN_CHAR}'	{ yylval.s = strdup(yytext); return CHAR_CONSTANT; }

\"{CHAR_IN_STR}*\"				{ yylval.s = strdup(yytext); return STR_CONSTANT; }

{ALPHA}({ALPHA}|{DIGIT})*		{ yylval.s = strdup(yytext); return NAME; }

[ \t]+							
\n								{ yylineno++; b_row_start = yytext + yyleng; b_col = 1; B_dwarf_statement(); }

"/\*"([^*]|\*+[^*/])*\*+"/"

.								{ if (!b_row_start) b_row_start = yytext; return yytext[0]; }

%%
