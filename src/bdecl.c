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
    String  tag = tmp_sprintf("%s + %d", name, C.back.word_size);

    asm_directive(DIR_LABEL, _OP_LABEL(name));
    asm_directive(DIR_DWORD, _OP_LABEL(strdup(tag))); // remove strdup and do a string arena please
    asm_prologue();
    
    scope_enter();
    symbol_new(name, SYM_FUNCTION);

    B.function.name = name;
    B.function.scope = vec_last(B.scopes);
}

void
B_function_stop(void)
{
    String  end = tmp_sprintf(".%s.end", B.function.name);

    asm_directive(DIR_LABEL, _OP_LABEL(strdup(end)));
    asm_epilogue();
    
    scope_exit();
    B.function = (Function) {0};
}

void
B_function_param(String name)
{
    symbol_new(name, SYM_PARAMETER);
    B.function.param_count++;
}

void
B_auto_decl(void)
{
}

void
B_auto_var(String name)
{
    symbol_new(name, SYM_VARIABLE);
    B.function.autos_count++;
}
