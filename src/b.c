/**
 * b.c
 */

#include <unistd.h>
#include <stdio.h>

#define B_NO_PREFIX
#include <btypes.h>
#include <bcontext.h>
#include <bsymbol.h>
#include <bdecl.h>
#include <bcodegen.h>

#include <b_i386.h>

extern int  yylex();
extern int  yylex_destroy(void);

extern int  yyparse();
extern FILE *yyin;

int yyerror(const char *s);

int
main(int argc, char **argv)
{
    int     ret;
    String  exe = SHIFT_ARGS(argc, argv);
    UNUSED(exe);

    compiler_start();
    yyin = stdin;

    if (argc)
        yyin = fopen(SHIFT_ARGS(argc, argv), "r");
    else
        B_warning("no input file. switching to stdin");

    if (!yyin)
    {
        B_error("can't open file '%s'", argv[1]);
        return (1);
    }

    C_builder_start(i386_setup);
    ret = yyparse();
	fclose(yyin);

	yylex_destroy();
	compiler_stop();

	return (ret);
}

int
yyerror(const char *s)
{
    fprintf(stderr, "[B] %s\n", s);
    return (1);
}
