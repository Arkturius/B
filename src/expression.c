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

Expression
B_expression_variable(StringC name)
{
	Symbol	*symbol = B_symbol_find(name);

	if (!symbol)
		B_error(ERROR_SYMBOL, "use of unknown identifier '%s'", name);

	return (Expression) { .type = EXPR_MEMORY, .mem = MEM_STACK(symbol->off) };
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
	return (Expression) { .type = 1, .imm = char_constant };
}

static Expression
B_expression_constant_string(StringC str)
{
	BTODO("String constants: allocation and expression.");
}

Expression
B_expression_constant(u64 value, StringC str, bool is_char)
{
	if (!str)
		return (Expression) { .type = EXPR_IMMEDIATE, .imm = value };
	if (!is_char)
		return (B_expression_constant_string(str + 1));
	else
		return (B_expression_constant_char(str + 1));
}

Expression
B_expression_assignment(u32 type, Expression lhs, Expression rhs)
{
	if (type != ASSIGN_OP)
		BTODO("handle assignment + operator.");

	if (lhs.type == EXPR_IMMEDIATE)
		B_error(ERROR_SYNTAX, "lvalue needed at the left of an assignment.");

	code_move(lhs, rhs);
	if (rhs.type == EXPR_REGISTER)
		register_free(rhs.reg);

	return (lhs);
}

Expression
B_expression_binop(BinopType type, Expression a, Expression b)
{
	Expression	dst = REG(register_alloc(REG_NULL));

	code_binop(type, dst, a, b);
	return (dst);
}


