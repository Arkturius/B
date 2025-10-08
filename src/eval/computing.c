/**
* computing.c
*/

#include "eval/expression.h"
#include <b.h>

Expression
B_compute_ternary(Expression a, Expression b, Expression c)
{
	B_DBG_TREE;

	todo("%s", __func__);
}

Expression
B_compute_binary_op(BOpType type, Expression a, Expression b)
{
	B_DBG_TREE;

	switch (type)
	{
		case BOP_PLUS:
		case BOP_MINUS:
			CG_binop(type, a, b);
			break ;
		default:
			todo("%s", __func__);
	}
	return (a);
}

Expression
B_compute_comparison(BOpType type, Expression a, Expression b)
{
	B_DBG_TREE;

	switch (type)
	{
		case BOP_EQ:
		case BOP_GT:
		case BOP_GE:
		case BOP_LT:
		case BOP_LE:
		case BOP_NE:
			CG_compare(a, b);
			break ;
		default:
			todo("handle more conditions in %s", __func__);
	}
	Expression	result = EA_allocate_comparison(type);

	return (result);
}

Expression
B_compute_unary_op(UOpType type, Expression a)
{
	B_DBG_TREE;

	switch (type)
	{
		case UOP_INCR:
		{
			Expression	b = EA_allocate_immediate(1);
			Expression	dst = EA_expr_copy(a);

			CG_binop(BOP_PLUS, a, b);
			CG_move(dst, a);
			break ;
		}
		default:
			unreachable("invalid UOpType. (%s)", x_tostr_UOpType(type));
	}
//	todo("%s", __func__);
}

