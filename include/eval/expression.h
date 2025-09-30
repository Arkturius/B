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

#endif // _B_EXPRESSION_H
