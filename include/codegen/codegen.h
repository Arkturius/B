/**
 * codegen.h
 */

#if !defined (_B_CODEGEN_H)
# define _B_CODEGEN_H

# define XLIB_NO_PREFIX
# include <xlib.h>

# include <eval/expression.h>

void
CG_program_header(void);


void
CG_rodata_section(void);

void
CG_data_section(void);


void
CG_data_scalar_list(StringC name);

void
CG_data_vector(StringC name, Size size);


void
CG_label(StringC label, bool user);

void
CG_align(Size align);


void
CG_function(StringC name);

void
CG_return(Expression e);

void
CG_prolog(void);

void
CG_debug_prolog(void);

void
CG_epilog(void);

void
CG_debug_epilog(void);


void
CG_stack_reserve(Size size);

void
CG_stack_release(Size size);


void
CG_expr_condition(Expression e);

void
CG_move(Expression dst, Expression src);

void
CG_ternary(Expression pred, Expression yes, Expression no);

void
CG_binop(BOpType op, Expression a, Expression b, bool in_place);

void
CG_compare(Expression e);

void
CG_test(Expression e);

void
CG_jump_label(StringC lbl, BOpType type);

void
CG_jump_compare(StringC lbl, Expression e);

void
CG_subscript(Expression arr, Expression idx);

void
CG_function_call(Expression e);

void
CG_function_arg(Expression e);

void
CG_invert(Expression e);

void
CG_negate(Expression e);

void
CG_incr(Expression e);

void
CG_decr(Expression e);

void
CG_post_incr(Expression e);

void
CG_post_decr(Expression e);

void
CG_deref(Expression e);

void
CG_addrof(Expression e);

void
CG_char(Expression s, Expression i);

void
CG_lchar(Expression s, Expression i, Expression c);

#endif // _B_CODEGEN_H
