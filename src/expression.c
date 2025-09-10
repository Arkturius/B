/**
 * expr.c
 */

#include <b.h>

# define B_ESCAPE_CHAR	'\\'

Expression
B_expression_primary(String identifier)
{
	return (Expression) {0};
}

# define	CONST_CHAR_ADD(_const, _x)	{ _const <<= 8; _const |= _x; }

static Expression
B_expression_constant_char(String str)
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
B_expression_constant_string(String str)
{
	BTODO("String constants: allocation and expression.");
}

Expression
B_expression_constant(u64 value, String str, bool is_char)
{
	if (!str)
		return (Expression) { .type = 1, .imm = value };
	if (!is_char)
		return (B_expression_constant_string(str + 1));
	else
		return (B_expression_constant_char(str + 1));
}
