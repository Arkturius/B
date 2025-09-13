/**
 * b.c
 */

#include <b.h>

extern int  yylex();
extern int  yylex_destroy(void);

extern int  yyparse();
extern FILE *yyin;

static StringC	executable	= NULL;
static StringC	input_file	= NULL;

Compiler		B = {0};

bool
B_compiler_start(void)
{
	arr_reserve(B.symbols, 16);
	arr_reserve(B.scopes, 16);

	return (true);
}

bool
B_compiler_stop(void)
{
	arr_destroy(B.symbols);
	arr_destroy(B.scopes);

    return (true);
}

FILE
*B_compiler_input(StringC filename)
{
	FILE	*input = stdin;

	if (filename)
        input = fopen(input_file, "r");
    if (!input)
		B_error(ERROR_FILE, "failed to open '%s'", input_file);
	return (input);
}

static int
B_compiler_run(void)
{
	int ret = yyparse();
	
    fclose(yyin);
	yylex_destroy();

	return (ret);
}

void
B_error_opt(ErrorType t, StringC fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	exit(1);
}

int
yyerror(const char *s)
{
	B_error_opt(ERROR_SYNTAX, "parsing error: %s", s);
	return (1);
}

int
main(int argc, char **argv)
{
	int	ret = 0;

	executable	= shift_args(argc, argv);
	input_file	= shift_args(argc, argv);

	yyin		= B_compiler_input(input_file);

	BLOG("compiler starting - file:'%s'", input_file);

	B_compiler_start();
	ret = B_compiler_run();
	B_compiler_stop();

	return (ret);
}
