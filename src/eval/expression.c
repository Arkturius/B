/**
* expression.c
*/

# include <b.h>
# include <codegen/regalloc.h>

static inline StringC
B_constant_string_name(void)
{
	static int	str_no = 0;

	return (B_asprintf(".ro%d", str_no++));
}

static inline Expression
B_eval_constant_string(StringC str)
{
	Expression	expr = arr_count(EA);

	String		lit = strndup(str + 1, strlen(str) - 2);
	ROString	new =
	{
		.name = B_constant_string_name(),
		.content = B_asprintf("%s", lit),
	};
	free(lit); // TODO: remove this when the string arena kicks in.

	arr_append(B.rostrings, new);
	return (expr);
}

static inline Expression
B_eval_constant_char(StringC str)
{
	Expression	expr = arr_count(EA);
	todo("evaluate constant chars.");
	return (expr);
}

static Expression
B_eval_constant_strlit(StringC str)
{
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
	Expression	expr = arr_count(EA);
	ExprAlloc	new  = { .op = IMM_OPERAND(imm) };

	arr_append(EA, new);
	return (expr);
}

Expression
B_eval_constant(i32 imm, StringC str)
{
	if (str)
		return (B_eval_constant_strlit(str));
	return (B_eval_constant_int(imm));
}

Expression 
B_eval_identifier(StringC identifier)
{
	Symbol	*symbol = B_symbol_find(identifier);
	Symbol	internal;

	if (!symbol)
	{
		StringC	internal_name = B_asprintf("%s.%s", B.function_name, identifier);
		internal = (Symbol)
		{
			.stype = STORAGE_INTERN,
			.vtype = VARIABLE_SCALAR,
			.name  = internal_name,
		};
		B_symbol_internal_add(&internal);
		symbol = &internal;
	}

	Expression	expr = EA_allocate_symbol(symbol);

	return (expr);
}

Expression
B_eval_assignment(OpType type, Expression dst, Expression src)
{
	switch (type)
	{
		case OP_NONE:
		{
			CG_move(dst, src);
			break ;
		}
		default:
			todo("implement %s case for %s", __func__, x_tostr_OpType(type));
	}
}
