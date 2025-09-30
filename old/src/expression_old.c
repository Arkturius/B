/**
 * expr.c
 */

#include <b.h>
#include <codegen.h>

# define B_ESCAPE_CHAR	'\\'

Expression
B_expression_variable(StringC name)
{
	Symbol	*symbol = B_symbol_find(name);

	if (!symbol)
		B_error(ERROR_SYMBOL, "use of unknown identifier '%s'.", name);

	if (symbol->type == SYMBOL_FUNCTION || symbol->type == SYMBOL_EXTERN)
		return SYM(name);

	Expression	var =
	{
		.type = EXPR_MEMORY,
		.mem = MEM_STACK(symbol->off)
	};
	if (symbol->size > WORD_SIZE)
	{
		Expression	tmp = code_load(var);
		
		return (tmp);
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
	lhs = B_lvalue(lhs);

	switch (type)
	{
		case ASSIGN_OP:
			code_move(lhs, rhs);
			break;
		case ASSIGN_OP_PLUS:
		case ASSIGN_OP_MINUS:
		{
			if (rhs.type == EXPR_MEMORY && lhs.type == EXPR_MEMORY)
			{
				Register	tmp = register_alloc(REG_NULL);

				code_move(REG(tmp), rhs);
				rhs = REG(tmp);
			}
			code_binop((BinopType)type, lhs, lhs, rhs);
			break ;
		}
		default:
			BTODO("handle assignment + operator.");
	}
	return (lhs);
}

Expression
B_expression_binop(BinopType type, Expression a, Expression b)
{
	Expression	dst = {0};

	if (type >= BINOP_LSHIFT)
	{
		if (type == BINOP_MULT || type == BINOP_DIV || type == BINOP_MOD)
		{
			register_alloc(REG_EDX);
			dst = REG(register_alloc(REG_EAX));
		}
		else if (a.type != EXPR_REGISTER)
			dst = REG(register_alloc(REG_NULL));
		else
			dst = a;
	}
	else
	{
		dst = (Expression) { .comparison = type };
	
		if (a.type == EXPR_IMMEDIATE && b.type != EXPR_IMMEDIATE)
		{
			Expression tmp;

			tmp = a;
			a = b;
			b = tmp;
			dst.comparison = B_switch_comparison(type);
		}
	}
	code_binop(type, dst, a, b);
	register_restore(REG_EDX);
	return (dst);
}

Expression
B_expression_subscript(Expression arr, Expression idx)
{
	Register	r;

	if (arr.type != EXPR_REGISTER)
	{
		r = register_alloc(REG_NULL);
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

	asm_movzx(result, from);

	return (REG(result));
}

void
B_builtin_lchar(Expression str, Expression idx, Expression nchar)
{
	Register	r = register_alloc(REG_NULL);

	if (str.type != EXPR_REGISTER)
	{
		code_move(REG(r), str);
		str.reg = r;
	}

	Memory		to  = MEM(.base = str.reg, .size = 1);
	Register	tmp = idx.reg;

	switch (idx.type)
	{
		case EXPR_IMMEDIATE:
			to.displacement = idx.imm;
			break ;
		case EXPR_MEMORY:
		{
			tmp = register_alloc(REG_NULL);
			code_move(REG(tmp), idx);
		}
		/* fallthrough */
		case EXPR_REGISTER:
			to.index = tmp;
			break ;
		default:
			B_error(ERROR_ASM, "builtin_lchar: invalid string index.");
	}

	Register	new = register_alloc(REG_NULL);

	switch (nchar.type)
	{
		case EXPR_MEMORY:
			code_move(REG(new), nchar);
			nchar.reg = new;
		/* fallthrough */
		case EXPR_REGISTER:
			emit_and_reg_imm(nchar.reg, 0xff);
			if (nchar.reg > REG_EBX)
			{
				register_spill(REG_ECX);
				code_move(ECX, nchar);
				nchar.reg = REG_ECX;
			}
			emit_mov_mem_sym(to, register_bytes[nchar.reg]);
			break ;
		case EXPR_IMMEDIATE:
			emit_mov_mem_imm(to, nchar.imm & 0xff);
			break ;
		default:
			B_error(ERROR_ASM, "builtin_lchar: invalid new character.");
	}
	register_restore(REG_ECX);
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

Expression
B_expression_compare(BinopType t, Expression a, Expression b)
{
	switch (t)
	{
		default:
			B_error(ERROR_ASM, "todo: more comparisons");
	}
}
