/**
 * bcontext.c
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define B_NO_PREFIX
#include <btypes.h>
#include <bcontext.h>
#include <bsymbol.h>
#include <bcodegen.h>

BCompiler   B = {0};
char        BTMP[2048] = {0};

# define    B_FLAG(_b, _flag)   ((_b).flags & _flag)

char
*B_tmp_sprintf(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vsnprintf((char *)BTMP, sizeof(BTMP), fmt, ap);
    va_end(ap);

    return (BTMP);
}

bool
B_compiler_start(void)
{
    memset(&B, 0, sizeof(BCompiler));
    B.flags |= B_INIT;
    return (true);
}

bool
B_compiler_stop(void)
{
    if (!B_FLAG(B, B_INIT))
        return (false);

    asm_dump();
    symbol_dump();

    vec_destroy(B.symtab);
    vec_destroy(B.rostrings);

    vec_map(String, B.symbols, free);
    vec_destroy(B.symbols);
    
    vec_map(String, B.rodata, free);
    vec_destroy(B.rodata);
    
    vec_destroy(B.scopes);

    return (true);
}

void
B_program_start(void)
{
    asm_directive(DIR_SYNTAX, (Operand){0});
    asm_directive(DIR_SECTION, _OP_LABEL(".text"));
}

void
B_program_stop(void)
{
    // TODO : Dump all ro data strings.
}

