/**
 * B codegen tool.
 */

#ifndef _BCODEGEN_H
# define _BCODEGEN_H

# include <types.h>

# define	_GEN(X, ...)		printf(X"\n", ##__VA_ARGS__)
# define	GEN(X, ...)			_GEN(X, ##__VA_ARGS__)
# define	GEN_OP(X)			_GEN("    "X)
# define	GEN_OP1(X, o1)		_GEN("    %s%*.s%s", X, 8 - (int)strlen(X), "", o1)
# define	GEN_OP2(X, o1, o2)	_GEN("    %s%*.s%s, %s", X, 8 - (int)strlen(X), "", o1, o2)
# define	GEN_LBL(X)			_GEN("%s:", X)

void	b_gen_section(SectionType t);

void	b_gen_program(void);

void	b_gen_function_start(String name);

void	b_gen_function_end(void);

#endif
