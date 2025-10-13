/**
* computing.h - Arithmetics and logic.
*/

#if !defined (_B_COMPUTING_H)
# define _B_COMPUTING_H

#include <eval/expression.h>

Expression
B_compute_comparison(BOpType type, Expression a, Expression b);

Expression
B_compute_ternary(Expression a, Expression b, Expression c);

Expression
B_compute_binary_op(BOpType type, Expression a, Expression b);

Expression
B_compute_unary_op(UOpType type, Expression a);

Expression
B_compute_postfix(UOpType type, Expression a);

#endif // _B_COMPUTING_H

