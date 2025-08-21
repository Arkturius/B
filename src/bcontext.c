/**
 * bcontext.c
 */

#include <string.h>

#define B_NO_PREFIX
#include <btypes.h>
#include <bcontext.h>
#include <bsymbol.h>

BCompiler   B = {0};

# define    B_FLAG(_b, _flag)   ((_b).flags & _flag)

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

}

void
B_program_stop(void)
{

}

