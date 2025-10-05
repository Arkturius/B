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
CG_data_scalar_list(void);

void
CG_data_vector(Size size);


void
CG_label(StringC label, bool user);

void
CG_align(Size align);


void
CG_function(StringC name);

void
CG_prolog(void);

void
CG_epilog(void);


void
CG_stack_reserve(Size size);

void
CG_stack_release(Size size);


void
CG_move(Expression dst, Expression src);

#endif // _B_CODEGEN_H
