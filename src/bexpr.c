/**
 * bexpr.c
 */

#include <bparser.h>

#define B_NO_PREFIX
#include <btypes.h>
#include <bcontext.h>
#include <bsymbol.h>
#include <bexpr.h>
#include <bcodegen.h>

Operand
B_expr_to_operand(Expr e)
{
    switch (e.type)
    {
        case EXPR_PREGISTER:
            return _OP_PREG(e.reg);
        case EXPR_VREGISTER:
            return _OP_VREG(e.imm);
        case EXPR_IMMEDIATE:
            return _OP_IMM(e.imm);
        case EXPR_VARIABLE:
            return _OP_MEM(e.off);
        case EXPR_ROSTRING:
        case EXPR_FUNCTION:
            return _OP_LABEL(e.name);
        default:
            break ;
    }
    B_error("unexpected ExprType.");
    compiler_stop();
    exit(1);
}

Expr
B_expr_variable(String name)
{
    Symbol  *sym = symbol_lookup(name);

    if (!sym)
    {
        B_error("use of undeclared identifier '%s'", name);
        compiler_stop();
        exit(1);
    }

    switch (sym->type)
    {
        case SYM_VARIABLE:
        case SYM_PARAMETER:
            return (Expr) { .type = EXPR_VARIABLE, .off = sym->off, .lval = true };
        case SYM_FUNCTION:
            return (Expr) { .type = EXPR_FUNCTION, .name = sym->name, .lval = false };
        case SYM_EXTERNAL:
        case SYM_LABEL:
            B_error("not implemented yet.");
            break ;
    }
    abort();
}

Expr
B_expr_assign(BinopType type, Expr lhs, Expr rhs)
{
    if (!lhs.lval)
    {
        B_error("left value needed on the left side of an assignment.");
        compiler_stop();
        exit(1);
    }

    if (type != BINOP_NULL)
        rhs = expr_binop(type, lhs, rhs);
    
    Operand dst = expr_to_operand(lhs);
    Operand src = expr_to_operand(rhs);

    asm_move(dst, src);

    if (src.type == OP_PREG || src.type == OP_VREG)
        register_free(src);

    return (lhs);
}

Expr
B_expr_binop(BinopType type, Expr a, Expr b)
{
    Expr    dst = register_alloc(); //TODO : register allocation

    Operand x = expr_to_operand(dst);
    Operand y = expr_to_operand(a);
    Operand z = expr_to_operand(b);

    asm_op_bin(type, x, y, z);

    if (y.type == OP_PREG || y.type == OP_VREG)
        register_free(y);
    if (z.type == OP_PREG || z.type == OP_VREG)
        register_free(z);

    return (dst);
}
