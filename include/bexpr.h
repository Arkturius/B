/**
 * bexpr.h
 */

#if !defined (_BEXPR_H)
# define _BEXPR_H

# include <btypes.h>
# include <bcodegen.h>

# if defined (B_NO_PREFIX)
#  define   expr_to_operand B_expr_to_operand
#  define   expr_variable   B_expr_variable
#  define   expr_assign     B_expr_assign
#  define   expr_binop      B_expr_binop
# endif

Operand
B_expr_to_operand(Expr e);

Expr
B_expr_variable(String name);

Expr
B_expr_assign(BinopType type, Expr lhs, Expr rhs);

Expr
B_expr_binop(BinopType type, Expr a, Expr b);

#endif // _BEXPR_H
