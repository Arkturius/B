/**
 * bdecl.c
 */

#define B_NO_PREFIX
#include <btypes.h>
#include <bcontext.h>
#include <bsymbol.h>

void
B_function_start(String name)
{
    symbol_new(name, SYM_FUNCTION);
    scope_enter();
}

void
B_function_stop(void)
{
    scope_exit();
}
