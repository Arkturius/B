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
	Expression	expr = arr_count(RA);

	String		lit = strndup(str + 1, strlen(str) - 2);
	ROString	new =
	{
		.name = B_constant_string_name(),
		.content = B_asprintf("%s", lit),
	};
	free(lit); // TODO: remove this when the string arena kicks in.

	arr_append(B.rostrings, new);

#if defined(B_DUMP_ALL)

	log("ROSTRING: name = %s", new.name);
	log("ROSTRING: data = \"%s\"", new.content);

#endif

	return (expr);
}

static inline Expression
B_eval_constant_char(StringC str)
{
	Expression	expr = arr_count(RA);
	todo("evaluate constant chars.");
	return (expr);
}

static Expression
B_eval_constant_strlit(StringC str)
{
	RegAlloc	new;

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
	Expression	expr = arr_count(RA);
	RegAlloc	new  = { .op = IMM_OPERAND(imm) };

	arr_append(RA, new);
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

	if (!symbol)
		unreachable("Use of undeclared identifier '%s'.", identifier);

	switch (symbol->type)
	{
		case SYMBOL_VAR_GLOBAL:
			todo("case SYMBOL_VAR_GLOBAL");
		case SYMBOL_VAR_LOCAL:
			todo("case SYMBOL_VAR_LOCAL");
		case SYMBOL_PARAMETER:
			todo("case SYMBOL_PARAMETER");
		case SYMBOL_FUNCTION:
			todo("case SYMBOL_FUNCTION");
		case SYMBOL_LABEL:
			todo("case SYMBOL_LABEL");
	
		case SYMBOL_TEMPORARY :
		default:
			unreachable("Invalid SymbolType. aborting.");
	}
	return (EXPR_INVALID);
}
