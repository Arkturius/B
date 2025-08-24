/**
 * bcontrol.c
 */

#include <string.h>

#define B_NO_PREFIX
#include <btypes.h>
#include <bcontext.h>
#include <bexpr.h>
#include <bcontrol.h>
#include <bcodegen.h>

void
B_return_expr(Expr ret)
{
    String  end = tmp_sprintf(".%s.end", B.function.name);
    
    Operand src = B_expr_to_operand(ret);

    asm_move(_OP_PREG("eax"), src);
    asm_jump(_OP_LABEL(strdup(end)));
}
