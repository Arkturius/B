/**
* declaration.h
*/

#if !defined (_B_DECLARATION_H)
# define _B_DECLARATION_H

# define XLIB_NO_PREFIX
# include <xlib.h>

# include <eval/expression.h>

typedef struct	b_ival
{
	Expression	expr;
	i32			imm;
}	IVal;

x_array(IVal, IVals);

void
B_eval_auto_decl(StringC name, Size size);

void
B_eval_auto_list(void);

void
B_eval_extern_decl(StringC name);

void
B_eval_extern_list(void);

void
B_eval_param_decl(StringC name);

void
B_eval_param_list(void);


void
B_eval_function_def(StringC name);

void
B_eval_intern_def(StringC name);

void
B_eval_simple_def(StringC name);

void
B_eval_vector_def(StringC name, Size size);


void
B_eval_ival(Expression e);

void
B_eval_ival_end(void);

#endif // _B_DECLARATION_H
