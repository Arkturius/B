/**
 * B compiler main header.
 */

#ifndef _BCOMPILER_H
# define _BCOMPILER_H

# include <types.h>
# include <bcodegen.h>

extern	BCompiler BC;

# if defined(BCOMPILER_IMPL)

BCompiler	BC;

void	b_program(void)
{
	memset(&BC, 0, sizeof(BC));
	b_gen_program();
}

void	b_function_start(String name)
{
	Function	f = (Function) 
	{
		.name = name,
		.type = SYMBOL_FUNCTION,
		.size = 0,
		.off = 0,
	};
	B_TABLE_PUSH(BC.functions, f);

	b_gen_function_start(name);
}

void	b_function_end(void)
{
	b_gen_function_end();
}

Expression	b_expr_constant(ConstantType t, i64 i, String s)
{
	switch (t)
	{
		case CONSTANT_INT:
		case CONSTANT_CHR:
			return (Expression) { .type = EXPR_CONSTANT, .ival = i };
		case CONSTANT_STR:
		{
			char	c = 0;

			if (strlen(s) == 3)
				c = s[1];
			else if (strlen(s) == 4 && s[1] == '\\')
			{
				switch (s[2])
				{
					case '0': c = '\0'; break ;
					case 'e': c = '\0'; break ;
					case '(': c = '{'; break ;
					case ')': c = '}'; break ;
					case 'n': c = '\n'; break ;
					case 't': c = '\t'; break ;
					case '*': c = '*'; break ;
					case '\"': c = '\"'; break ;
					case '\'': c = '\''; break ;
					default: c = s[2]; break ;
				}
			}
			return (Expression) { .type = EXPR_STRING, .str = s };
		}
		default:
			b_fatal("unexpected constant type", __func__);
	}
	return (Expression) {0};
}

Expression	b_expr_autovar(String name)
{
	return (Expression) { .type = EXPR_AUTOVAR, .str = name };
}

Expression	b_expr_array(Expression arr, Expression i)
{
	// TODO: emit a lea [arr + 4 * i]

	return (Expression) {0};
}

Expression	b_builtin_char(Expression s, Expression i)
{
	// TODO: emit a movzx dst, [s + i];

	return (Expression) {0};
}

Expression	b_builtin_lchar(Expression s, Expression i, Expression c)
{
	// TODO: emit a mov byte [s + i], c

	return (Expression) {0};
}

# endif // BCOMPILER_IMPL

#endif // _BCOMPILER_H
