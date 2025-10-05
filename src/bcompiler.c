/**
* bcompiler.c
*/

#include "symbols.h"
#include <stdarg.h>

#include <b.h>

extern int		yylex();
extern int  	yylex_destroy(void);
extern int  	yyparse();
extern FILE 	*yyin;

static StringC	executable = NULL;
static StringC	input_file = NULL;
Compiler		B = {0};

_constructor() bool
B_compiler_start()
{
	arr_reserve(B.symbols, 256);
	arr_reserve(B.scopes,  256);
}

_destructor() void
B_compiler_stop()
{
	static bool	dead = false;

	if (dead)
		return ;
	dead = true;

	arr_destroy(B.symbols);
	arr_destroy(B.scopes);
}

bool
B_compiler_run(void)
{
	bool ret = yyparse();

	fclose(yyin);
	yylex_destroy();

	log("SYMBOL TABLE AT END OF COMPILATION:");
	B_symbol_table_dump();
	return (ret);
}

_noreturn void
B_compiler_error(StringC reason)
{
	todo("%s: BETTER ERROR HANDLING", reason);

	StringC	line = b_row_start ? b_row_start : yytext;

	yytext[yyleng] = b_hold_char;

	StringC	nl = strchr(line, '\n');

	if (!nl)
		nl = line + strlen(line);

	log("\033[1m%s:%d:%d:\033[0m ", input_file, yylineno, b_col - (yyleng - 1));
	log(" %4d | %.*s", yylineno, (int)(nl - line), line);
	
	String	waves = malloc(b_col - yyleng + 1);
	if (!waves)
		exit(42069);
	
	memset(waves, '~', b_col - yyleng);
	waves[b_col - yyleng - 1] = 0;

	log("      | %*.s^\033[35;1m%s\033[0m", (int)b_col - 1, "", waves);

	todo("%s: %s", __func__, reason);
	B_compiler_stop();
	exit(1);
}

StringC
B_asprintf(StringC fmt, ...)
{
	char	buffer[2048] = {0};
	va_list	ap;

	va_start(ap, fmt);
	vsnprintf(buffer, sizeof(buffer) - 1, fmt, ap);
	va_end(ap);

	return (strdup(buffer));
}

int
yyerror(StringC reason)
{
	B_compiler_error(reason);
	return (1);
}

int
main(int argc, char **argv, char **envp)
{
	executable = shift_args(argc, argv);
	if (argc)
		input_file = shift_args(argc, argv);
	
	yyin = input_file ? fopen(input_file, "r") : stdin;

	if (!yyin)
		B_compiler_error("No such file or directory.");

	if (!B_compiler_run())
		return (1);
}
