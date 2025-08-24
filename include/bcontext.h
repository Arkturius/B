/**
 * bcontext.h
 */

#if !defined(_BCONTEXT_H)
# define _BCONTEXT_H

# include <btypes.h>

typedef enum    b_flags
{
    B_NULL,
    B_INIT,
}   BFlags;

typedef struct  b_compiler
{
    BFlags      flags;

    Symbols     symtab;
    Strings     symbols;

    RoStrings   rostrings;
    Strings     rodata;

    Scopes      scopes;
    Function    function;
    Loop        loops;

}   BCompiler;

extern  BCompiler   B;

# if defined(B_NO_PREFIX)
#  define   tmp_sprintf         B_tmp_sprintf
#  define   compiler_start      B_compiler_start
#  define   compiler_stop       B_compiler_stop
#  define   program_start       B_program_start
#  define   program_stop        B_program_stop
# endif

char
*B_tmp_sprintf(const char *fmt, ...);

bool
B_compiler_start(void);

bool
B_compiler_stop(void);

/* Program specific */

void
B_program_start(void);

void
B_program_stop(void);

#endif // _BCONTEXT_H
