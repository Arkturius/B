/**
* computing.c
*/

#include <b.h>

Expression
B_compute_ternary(Expression a, Expression b, Expression c)
{
	B_DBG_TREE;

	CG_ternary(a, b, c);
	return (a);
}

Expression
B_compute_binary_op(BOpType type, Expression a, Expression b)
{
	B_DBG_TREE;

	switch (type)
	{
		case BOP_PLUS:
		case BOP_MINUS:
		case BOP_AND:
		case BOP_OR:
		case BOP_SHL:
		case BOP_SHR:
			CG_binop(type, a, b, false);
			break ;
		case BOP_EQ:
		case BOP_GT:
		case BOP_GE:
		case BOP_LT:
		case BOP_LE:
		case BOP_NE:
		{
			Expression	result = EA_allocate_comparison(type, a, b);

			return (result);
		}
		default:
			todo("%s", __func__);
	}
	return (a);
}

Expression
B_compute_unary_op(UOpType type, Expression a)
{
	B_DBG_TREE;

	switch (type)
	{
		case UOP_INCR:
			CG_incr(a);
			break ;
		case UOP_DECR:
			CG_decr(a);
			break ;
		case UOP_ADDR:
			CG_addrof(a);
			break ;
		case UOP_STAR:
			CG_deref(a);
			break ;
		case UOP_NOT:
			CG_invert(a);
			break ;
		case UOP_NEG:
			CG_negate(a);
			break ;
		default:
			unreachable("invalid UOpType. (%s)", x_tostr_UOpType(type));
	}
	return (a);
}

Expression
B_compute_postfix(UOpType type, Expression a)
{
	B_DBG_TREE;

	if (type == UOP_INCR)
		CG_post_incr(a);
	else if (type == UOP_DECR)
		CG_post_decr(a);
	else
		unreachable("invalid postfix operator.");
	return (a);
}
