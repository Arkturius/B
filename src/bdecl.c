/**
 * bdecl.c
 */

#include <string.h>

#define B_NO_PREFIX
#include <btypes.h>
#include <bcontext.h>
#include <bsymbol.h>
#include <bcodegen.h>

void
B_function_start(String name)
{
    symbol_new(name, SYM_FUNCTION);
    scope_enter();

    printf("the function has for name %s\n", name);

    String  tag = tmp_sprintf("%s + %d", name, C.back.word_size);

    asm_directive(DIR_LABEL, _OP_LABEL(name));
    asm_directive(DIR_DWORD, _OP_LABEL(strdup(tag))); // remove strdup and do a string arena please
    asm_prologue();

    B.function.name = name;
    B.function.scope = vec_last(B.scopes);
}

void
B_function_stop(void)
{
    scope_exit();

    asm_epilogue();
}
