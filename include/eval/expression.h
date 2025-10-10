/**
* expression.h - Expression handling.
*/

#if !defined (_B_EXPRESSION_H)
# define _B_EXPRESSION_H

# define XLIB_NO_PREFIX
# include <xlib.h>

# define	WORD_SIZE		4
# define	EXPR_INVALID	(-1U)

typedef u32	Expression;

x_array(Expression,	 Expressions);
x_array(Expressions, ArgumentStack);

typedef struct b_rostring
{
	StringC	name;
	StringC	content;
}	ROString;

x_array(ROString, ROStrings);

Expression
B_eval_constant(i32 imm, StringC constant);

Expression 
B_eval_identifier(StringC identifier);

x_enum
(
	BOpType,
	x_enum_prefix(BOP),
	x_enum_members 
	(
		(NONE ),
		(PLUS ),
		(MINUS),
		(MULT ),
		(DIV  ),
		(MOD  ),
		(AND  ),
		(OR   ),
		(SHL  ),
		(SHR  ),
		(EQ   ),
		(NE   ),
		(LT   ),
		(GT   ),
		(LE   ),
		(GE   ),
	)
)

x_enum
(
	UOpType,
	x_enum_prefix(UOP),
	x_enum_members
	(
		(STAR),
		(ADDR),
		(NEG ),
		(NOT ),
		(INCR),
		(DECR),
	)
)

Expression
B_eval_assignment(BOpType type, Expression dst, Expression src);

Expression
B_eval_function_call(Expression f);

void
B_eval_function_arg(Expression arg);

void
B_eval_function_call_start(void);

Expression
B_eval_subscript(Expression arr, Expression idx);

Expression
B_builtin_char(Expression s, Expression i);

void
B_builtin_lchar(Expression s, Expression i, Expression c);

#endif // _B_EXPRESSION_H
