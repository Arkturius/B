/**
 * codegen.c
 */

#include "expression.h"
#include <b.h>
#include <codegen.h>

StringC syntax_names[SYNTAX_ENUM_MAX] = 
{
	[SYNTAX_INTEL]	= ".intel_syntax noprefix",
};

StringC	directive_names[DIRECTIVE_ENUM_MAX] = 
{
	[DIRECTIVE_SYNTAX]	= NULL,
	[DIRECTIVE_ALIGN]	= ".align",
	[DIRECTIVE_SECTION]	= ".section",
	[DIRECTIVE_GLOBAL]	= ".global",
	[DIRECTIVE_LONG]	= ".long",
};

StringC	section_names[SECTION_ENUM_MAX] = 
{
	[SECTION_TEXT]		= ".text",
	[SECTION_DATA]		= ".data",
	[SECTION_RODATA]	= ".rodata",
	[SECTION_BSS]		= ".bss",
};

static StringC
B_register(Register r)
{
	if (r >= REG_ENUM_MAX)
		return (NULL);
	return (register_names[r]);
}

void
asm_directive_opt(DirectiveType type, struct _asm_directive_opt opt)
{
	if (type >= DIRECTIVE_ENUM_MAX)
		B_error(ERROR_SYNTAX, "unknown directive. %d >= DIRECTIVE_ENUM_MAX", type);

	StringC	name = directive_names[type];

	if (name)
		printf("%s ", name);
	switch (type)
	{
		case DIRECTIVE_ALIGN:
			printf("%lu", opt.value);
			break ;
		case DIRECTIVE_SYNTAX:
		case DIRECTIVE_SECTION:
		case DIRECTIVE_GLOBAL:
			printf("%s", (String)opt.data);
			break ;
		case DIRECTIVE_LONG:
			printf("%s", (String)opt.data);
			if (opt.off)
				printf(" + %u", opt.off);
		default:
			break ;
	}
	printf("\n");
}

void
asm_label(StringC name)
{
	printf("%s:\n", name);
}

void
asm_store(Expression dst, Expression src)
{
	switch (dst.type)
	{
		case EXPR_REGISTER:
		case EXPR_MEMORY:
			asm_mov(dst, src);
			break ;
		default:
			B_error(ERROR_SYNTAX, "lvalue needed at the left of assignment.");
	}
}

StringC
asm_operand(Expression e)
{
	switch (e.type)
	{
		case EXPR_REGISTER:
			return (B_register(e.reg));
		case EXPR_IMMEDIATE:
		{
			StringC	op = B_asprintf("%d", e.imm);

			return (op);
		}
		case EXPR_MEMORY:
		{
			StringC	op = B_asprintf("");

			BTODO("memory operands.");
			return (op);
		}
		default:
			B_error(ERROR_SYNTAX, "unknown expression type.");
	}
}

void
asm_push(Expression p)
{
	StringC	op = asm_operand(p);

	printf("  push    %s\n", op);
}

void
asm_pop(Expression p)
{
	StringC	op = asm_operand(p);

	printf("  pop     %s\n", op);
}

void
asm_mov(Expression dst, Expression src)
{
	StringC	op1 = asm_operand(dst);
	StringC	op2 = asm_operand(src);

	printf("  mov     %s, %s\n", op1, op2);
}

void
asm_sub(Expression a, Expression b)
{
	StringC	op1 = asm_operand(a);
	StringC	op2 = asm_operand(b);

	printf("  sub     %s, %s\n", op1, op2);
}

void
asm_xor(Expression a, Expression b)
{
	StringC	op1 = asm_operand(a);
	StringC	op2 = asm_operand(b);

	printf("  xor     %s, %s\n", op1, op2);
}

void
asm_ret(void)
{
	printf("  ret\n");
}
