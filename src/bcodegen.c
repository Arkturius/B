/**
 * bcodegen.c
 */

#include <stdarg.h>

#include <btypes.h>
#include <bcontext.h>
#include <bcodegen.h>
#include <stdbool.h>

CodeBuilder C = {0};

bool
C_registers_set(Size count, ...)
{
    va_list ap;

    if (count == 0)
    {
        B_error("%s: no physical registers.", __func__);
        return (false);
    }

    va_start(ap, count);
    while (count)
    {
        String  reg = va_arg(ap, String);

        if (!reg)
        {
            B_error("%s: NULL register.", __func__);
            return (false);
        }
        vec_append(C.regs.physical, reg);
        count--;
    }
    va_end(ap);
    return (true);
}

bool
C_builder_start(FN_backend backend_setup)
{
    if (!backend_setup)
    {
        B_error("%s: no backend setup provided.", __func__);
        return (false);
    }
    return (backend_setup(&C.back));
}
