/**
* expression.c
*/

# include <b.h>
# include <codegen/regalloc.h>

static inline StringC
B_constant_string_name(void)
{
	B_DBG_TREE;

	static int	str_no = 0;

	return (B_asprintf("ROS%d", str_no++));
}

static inline Expression
B_eval_constant_string(StringC str)
{
	B_DBG_TREE;

	String		lit = strndup(str + 1, strlen(str) - 2);
	StringC		name = B_constant_string_name();
	ROString	new =
	{
		.name = name,
		.content = B_asprintf("%s", lit),
	};
	free(lit); // TODO: remove this when the string arena kicks in.
	arr_append(B.rostrings, new);

	Expression	expr = EA_allocate_string(name);
	return (expr);
}

static inline char
B_eval_escaped_char(StringC str)
{
	char	c = 0;

	switch (*str)
	{
		case '0':
		case 'e':  c = 0;    break ;
		case 't':  c = '\t'; break ;
		case 'n':  c = '\n'; break ;
		case '(':  c = '{';  break ;
		case ')':  c = '}';  break ;
		case '\\': c = '\\'; break ;
		case '\'': c = '\''; break ;
		case '"':  c = '"';  break ;
		default:
			B_compiler_error("invalid escape sequence.");
			break ;
	}
	return (c);
}

static inline Expression
B_eval_constant_char(StringC str)
{
	B_DBG_TREE;

	short	final = 0;
	char	c = 0;
	String	lit = strndup(str + 1, strlen(str) - 2);
	Size	len = strlen(lit);

	while (len--)
	{
		switch (*lit)
		{
			case '\\':
				lit++;
				len--;
				c = B_eval_escaped_char(lit);
				break ;
			default:
				c = *lit;
				break ;
		}
		final = (final << 8) | c;
		lit++;
	}

	Expression	expr = EA_allocate_immediate(final);
	return (expr);
}

static Expression
B_eval_constant_strlit(StringC str)
{
	B_DBG_TREE;

	ExprAlloc	new;

	unused(new);
	switch (*str)
	{
		case '\'':
			return (B_eval_constant_char(str));
		case '"':
			return (B_eval_constant_string(str));
	}
}

static Expression
B_eval_constant_int(i32 imm)
{
	B_DBG_TREE;

	Expression	expr = EA_allocate_immediate(imm); 
	return (expr);
}

Expression
B_eval_constant(i32 imm, StringC str)
{
	B_DBG_TREE;

	if (str)
		return (B_eval_constant_strlit(str));
	return (B_eval_constant_int(imm));
}

Expression 
B_eval_identifier(StringC identifier)
{
	B_DBG_TREE;

	Symbol	*symbol = B_symbol_find(identifier);

	if (!symbol)
	{
		StringC	internal_name = B_asprintf("%s.%s", B.function_name, identifier);
		B_eval_intern_def(internal_name);
		symbol = arr_last(B.internals);
	}

	Expression	expr = EA_allocate_symbol(symbol);

	return (expr);
}

Expression
B_eval_assignment(BOpType type, Expression dst, Expression src)
{
	B_DBG_TREE;

	switch (type)
	{
		case BOP_NONE:
			CG_move(dst, src);
			break ;
		case BOP_PLUS:
		case BOP_MINUS:
		case BOP_AND:
		case BOP_SHR:
			CG_binop(type, dst, src, true);
			break ;
		default:
			todo("implement %s case for %s", __func__, x_tostr_BOpType(type));
	}
	return (dst);
}

Expression
B_eval_subscript(Expression arr, Expression idx)
{
	B_DBG_TREE;

	CG_subscript(arr, idx);
	return (arr);
}

void
B_eval_function_call_start(void)
{
	B_DBG_TREE;

	Expressions	new_args = {0};

	arr_reserve(new_args, 8);
	arr_append(B.arguments, new_args);
}

Expression
B_eval_function_call(Expression e)
{
	B_DBG_TREE;

	CG_function_call(e);
	CG_stack_release(WORD_SIZE * arr_count(*arr_last(B.arguments)));

	arr_pop(B.arguments, 1);
	return (e);
}

void
B_eval_function_arg(Expression e)
{
	B_DBG_TREE;

	CG_function_arg(e);	
}

void
B_dwarf_statement(void)
{
	B_DBG_TREE;

#if defined(B_DEBUG)
	ASM_dir_loc("is_stmt 1");
#endif // B_DEBUG
}

Expression
B_builtin_char(Expression str, Expression idx)
{
	B_DBG_TREE;

	CG_char(str, idx);
	return (str);
}

void
B_builtin_lchar(Expression str, Expression idx, Expression c)
{
	B_DBG_TREE;

	CG_lchar(str, idx, c);
}
