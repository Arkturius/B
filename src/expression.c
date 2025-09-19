/**
 * expr.c
 */

#include "expression.h"
#include <b.h>
#include <codegen.h>

# define B_ESCAPE_CHAR	'\\'

StringC	register_names[REG_ENUM_MAX] = 
{
	[REG_EDX] = "edx",
	[REG_EAX] = "eax",
	[REG_ECX] = "ecx",
	[REG_EBX] = "ebx",
	[REG_EDI] = "edi",
	[REG_ESI] = "esi",
	[REG_ESP] = "esp",
	[REG_EBP] = "ebp",
};

StringC jump_ccs[COMP_ENUM_MAX] =
{
	[COMP_NONE]	= "jmp",
	[COMP_E]	= "je",
	[COMP_NE]	= "jne",
};

Expression
B_expression_variable(StringC name)
{
	Symbol	*symbol = B_symbol_find(name);

	if (!symbol)
		B_error(ERROR_SYMBOL, "use of unknown identifier '%s'", name);

	if (symbol->type == SYMBOL_FUNCTION || symbol->type == SYMBOL_EXTERN)
		return (Expression) { .type = EXPR_SYMBOL, .sym = name };

	Expression	var =
	{
		.type = EXPR_MEMORY,
		.mem = MEM_STACK(symbol->off)
	};

	if (symbol->size > WORD_SIZE)
	{	
		Register	tmp = register_alloc(REG_NULL);
	
		code_load(REG(tmp), var);
		return (REG(tmp));
	}

	return (var);
}

# define	CONST_CHAR_ADD(_const, _x)	{ _const <<= 8; _const |= _x; }

static Expression
B_expression_constant_char(StringC str)
{
	Size	len = strlen(str) - 1;
	short	char_constant = 0;

	while (len--)
	{
		switch (*str)
		{
			case B_ESCAPE_CHAR:
				str++;
				len--;
				switch (*str)
				{
					case '0':
					case 'e':
						CONST_CHAR_ADD(char_constant, 0);
						break ;
					case 't':
						CONST_CHAR_ADD(char_constant, '\t');
						break ;
					case 'n':
						CONST_CHAR_ADD(char_constant, '\n');
						break ;
					case '(':
						CONST_CHAR_ADD(char_constant, '{');
						break ;
					case ')':
						CONST_CHAR_ADD(char_constant, '}');
						break ;
					case B_ESCAPE_CHAR:
						CONST_CHAR_ADD(char_constant, B_ESCAPE_CHAR);
						break ;
					case '\'':
						CONST_CHAR_ADD(char_constant, '\'');
						break ;
					case '"':
						CONST_CHAR_ADD(char_constant, '"');
						break ;
					default:
						B_error(ERROR_SYNTAX, "unknown escape sequence '\\%02x'", *str);
						break ;
				}
				break ;
			default:
				CONST_CHAR_ADD(char_constant, *str);
				break ;
		}
		str++;
	}
	return (Expression) { .type = EXPR_IMMEDIATE, .imm = char_constant };
}

static StringC
B_expression_string_label(void)
{
	return (B_asprintf(".rs%d", arr_count(B.rostrings)));
}

static Expression
B_expression_constant_string(StringC str)
{
	StringC	name = B_expression_string_label();

	RoString	rostr = 
	{
		.text = str,
		.name = name,
	};
	arr_append(B.rostrings, rostr);

	return (Expression) { .type = EXPR_SYMBOL, .sym = name };
}

Expression
B_expression_constant(u64 value, StringC str, bool is_char)
{
	if (!str)
		return (Expression) { .type = EXPR_IMMEDIATE, .imm = value };
	if (!is_char)
		return (B_expression_constant_string(str));
	else
		return (B_expression_constant_char(str + 1));
}

Expression
B_lvalue(Expression e)
{
	switch (e.type)
	{
		case EXPR_MEMORY:
			return (e);
		default:
			B_error(ERROR_SYNTAX, "needed lvalue for assignment.");
	}
}

Expression
B_deref(Expression addr)
{
	Expression	result = REG(register_alloc(REG_NULL));

	code_move(result, addr);
	return (result);
}

Expression
B_expression_assignment(AssignType type, Expression lhs, Expression rhs)
{
	Expression	res;

	lhs = B_lvalue(lhs);

	switch (type)
	{
		case ASSIGN_OP:
			code_move(lhs, rhs);
			break;
		case ASSIGN_OP_PLUS:
			res = B_expression_binop(BINOP_PLUS, lhs, rhs);
			code_move(lhs, res);
			break ;
		case ASSIGN_OP_MINUS:
			res = B_expression_binop(BINOP_MINUS, lhs, rhs);
			code_move(lhs, res);
			break ;
		default:
			BTODO("handle assignment + operator.");
	}

	if (lhs.type == EXPR_REGISTER)
		register_free(lhs.reg);
	if (lhs.type == EXPR_MEMORY && lhs.mem.base <= REG_USABLE)
	{
		register_free(lhs.mem.base);
		register_free(lhs.mem.index);
	}

	return (lhs);
}

Expression
B_expression_binop(BinopType type, Expression a, Expression b)
{
	Expression	dst = REG(register_alloc(REG_NULL));

	code_binop(type, dst, a, b);
	
	if (a.type == EXPR_REGISTER)
		register_free(a.reg);
	if (b.type == EXPR_REGISTER)
		register_free(b.reg);

	return (dst);
}

Expression
B_expression_subscript(Expression arr, Expression idx)
{
	Register	r = register_alloc(REG_NULL);

	if (arr.type != EXPR_REGISTER)
	{
		code_move(REG(r), arr);
		arr = REG(r);
	}

	Expression	addr = 
	{
		.type = EXPR_MEMORY,
		.mem  = MEM(.base = arr.reg),
	};

	Register	tmp = idx.reg;

	switch (idx.type)
	{
		case EXPR_IMMEDIATE:
			addr.mem.displacement = 4 * idx.imm;
			break ;
		case EXPR_MEMORY:
		{
			tmp = register_alloc(REG_NULL);
			code_move(REG(tmp), idx);
		}
		/* fallthrough */
		case EXPR_REGISTER:
			addr.mem.index = tmp;
			addr.mem.scale = 4;
			break ;
		default:
			B_error(ERROR_ASM, "invalid subscript index.");
	}

//	register_free(r);
	register_free(tmp);

	return (addr);
}


Expression
B_builtin_char(Expression str, Expression idx)
{
	Register	r = register_alloc(REG_NULL);

	if (str.type != EXPR_REGISTER)
	{
		code_move(REG(r), str);
		str.reg = r;
	}

	Memory		from  = MEM(.base = str.reg, .size = 1);
	Register	tmp = idx.reg;

	switch (idx.type)
	{
		case EXPR_IMMEDIATE:
			from.displacement = idx.imm;
			break ;
		case EXPR_MEMORY:
		{
			tmp = register_alloc(REG_NULL);
			code_move(REG(tmp), idx);
		}
		/* fallthrough */
		case EXPR_REGISTER:
			from.index = tmp;
			break ;
		default:
			B_error(ERROR_ASM, "builtin_char: invalid string index.");
	}
	Register	result = register_alloc(REG_NULL);

	register_free(r);
	register_free(tmp);
	register_free(str.reg);

	asm_movzx(result, from);

	return (REG(result));
}

Expression
B_expression_address(Expression e)
{
	switch (e.type)
	{
		case EXPR_MEMORY:
		case EXPR_SYMBOL:
		{
			Register	tmp = register_alloc(REG_NULL);

			code_load(REG(tmp), e);
//			register_free(e.reg);
			return (REG(tmp));
		}
		case EXPR_IMMEDIATE:
		case EXPR_REGISTER:
		default:
			B_error(ERROR_ASM, "can't take the address of this expression.'");
	}
}

Expression
B_expression_deref(Expression e)
{
	return (B_expression_subscript(e, IMM(0)));
}

Expression
B_expression_incr(Expression e)
{
	Register	tmp = register_alloc(REG_NULL);

	code_move(REG(tmp), e);
	B_expression_assignment(ASSIGN_OP_PLUS, e, IMM(1));

	return (REG(tmp));
}

Expression
B_expression_decr(Expression e)
{
	Register	tmp = register_alloc(REG_NULL);

	code_move(REG(tmp), e);
	B_expression_assignment(ASSIGN_OP_MINUS, e, IMM(1));

	return (REG(tmp));
}
