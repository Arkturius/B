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

void
emit_directive_opt(DirectiveType type, struct _emit_directive_opt opt)
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

# define	FMT_INSTR			"  %s%*.s"
# define	FMT_SPACING(_i)		(int)(8 - strlen(_i))

# if defined (B_DEBUG)
#  define	EMIT(_i, _o, ...)												\
	do																		\
	{																		\
		printf(FMT_INSTR _o"\n", _i, FMT_SPACING(_i), "", ##__VA_ARGS__);	\
		asm("int3");														\
	}																		\
	while (0);
# else
#  define	EMIT(_i, _o, ...)												\
		printf(FMT_INSTR _o"\n", _i, FMT_SPACING(_i), "", ##__VA_ARGS__)
# endif

void
emit_label(StringC label)
{
	printf("%s:\n", label);
}

StringC
emit_mem(Memory mem, bool sized)
{
	StringC	sizes[3] = {"DWORD", "BYTE", "WORD"};
	char	buffer[128] = {0};

	if (sized)
	{
		if (mem.size >= 3)
			B_error(ERROR_ASM, "unknown memory size.");
		strcat(buffer, B_asprintf("%s PTR ", sizes[mem.size]));
	}
	strcat(buffer, B_asprintf("[%s", register_names[mem.base]));
	if (mem.index != REG_NULL)
	{
		strcat(buffer, B_asprintf(" + %s", register_names[mem.index]));
		if (mem.scale > 1)
			strcat(buffer, B_asprintf(" * %d", mem.scale));
	}
	if (mem.displacement != 0)
	{
		if (mem.displacement < 0)
			strcat(buffer, B_asprintf(" - %d", -mem.displacement));
		else
			strcat(buffer, B_asprintf(" + %d", mem.displacement));
	}
	strcat(buffer, "]");

	B_arena_erase(strlen(buffer));

	return (B_asprintf("%s", buffer));
}

void
emit_push_imm(Immediate imm)
{
	EMIT("push", "%d", imm);
}

void
emit_push_reg(Register reg)
{
	EMIT("push", "%s", register_names[reg]);
}

void
emit_push_mem(Memory mem)
{
	StringC	rep = emit_mem(mem, false);

	EMIT("push", "%s", rep);
}

void
emit_pop_imm(Immediate imm)
{
	EMIT("pop", "%d", imm);
}

void
emit_pop_reg(Register reg)
{
	EMIT("pop", "%s", register_names[reg]);
}

void
emit_pop_mem(Memory mem)
{
	StringC	rep = emit_mem(mem, false);

	EMIT("pop", "%s", rep);
}

ASM_BINARY_OP_IMPL(mov);
ASM_BINARY_OP_IMPL(add);
ASM_BINARY_OP_IMPL(sub);

void
emit_ret()
{
	EMIT("ret", "");
}

void
emit_jmp_label(StringC label)
{
	EMIT("jmp", "%s", label);
}

/******************************************************************************/

void
asm_push(Expression e)
{
	switch(e.type)
	{
		case EXPR_IMMEDIATE:
			emit_push_imm(e.imm);
			break ;
		case EXPR_REGISTER:
			emit_push_reg(e.reg);
			break;
		case EXPR_MEMORY:
			emit_push_mem(e.mem);
			break ;
		default:
			B_error(ERROR_ASM, "invalid operand type for 'push'.");
	}
}

void
asm_pop(Expression e)
{
	switch(e.type)
	{
		case EXPR_IMMEDIATE:
			emit_pop_imm(e.imm);
			break ;
		case EXPR_REGISTER:
			emit_pop_reg(e.reg);
			break;
		case EXPR_MEMORY:
			emit_pop_mem(e.mem);
			break ;
		default:
			B_error(ERROR_ASM, "invalid operand type for 'pop'.");
	}
}

typedef enum _boperand_pair
{
	OP_REG_IMM,
	OP_REG_REG,
	OP_REG_MEM,
	OP_MEM_IMM,
	OP_MEM_REG,
	OP_INVALID,
}	OperandPair;

OperandPair
asm_operand_pair(Expression dst, Expression src)
{
	switch (dst.type)
	{
		case EXPR_REGISTER:
		switch (src.type)
		{
			case EXPR_IMMEDIATE: return (OP_REG_IMM);
			case EXPR_REGISTER:  return (OP_REG_REG);
			case EXPR_MEMORY:    return (OP_REG_MEM);
		}
		break ;
		case EXPR_MEMORY:
		switch (src.type)
		{
			case EXPR_IMMEDIATE: return (OP_MEM_IMM);
			case EXPR_REGISTER:  return (OP_MEM_REG);
			case EXPR_MEMORY:	 return (OP_INVALID);
		}
		break ;
		default:
			B_error(ERROR_ASM, "invalid operand type for 'mov'");
	}
}

# define	ASM_DISPATCH_BINARY(_a, _b, _instr)								\
																			\
	do																		\
	{																		\
		OperandPair	_pair = asm_operand_pair(_a, _b);						\
		switch (_pair)														\
		{																	\
			case OP_REG_IMM:												\
				emit_##_instr##_reg_imm(_a.reg, _b.imm);					\
				break ;														\
			case OP_REG_REG:												\
				emit_##_instr##_reg_reg(_a.reg, _b.reg);					\
				break ;														\
			case OP_REG_MEM:												\
				emit_##_instr##_reg_mem(_a.reg, _b.mem);					\
				break ;														\
			case OP_MEM_IMM:												\
				emit_##_instr##_mem_imm(_a.mem, _b.imm);					\
				break ;														\
			case OP_MEM_REG:												\
				emit_##_instr##_mem_reg(_a.mem, _b.reg);					\
				break ;														\
			case OP_INVALID:												\
			default:														\
				B_error(ERROR_ASM, "'%s': invalid operand type.", #_instr); \
		}																	\
	}																		\
	while (0)

void
asm_mov(Expression dst, Expression src)
{
	if (dst.type == EXPR_MEMORY && src.type == EXPR_MEMORY)
		B_error(ERROR_ASM, "'mov': memory to memory not supported.");

	ASM_DISPATCH_BINARY(dst, src, mov);
}

void
asm_add(Expression dst, Expression add)
{
	if (dst.type == EXPR_MEMORY && add.type == EXPR_MEMORY)
		B_error(ERROR_ASM, "'add': memory to memory not supported.");

	ASM_DISPATCH_BINARY(dst, add, add);
}

void
asm_sub(Expression dst, Expression sub)
{
	if (dst.type == EXPR_MEMORY && sub.type == EXPR_MEMORY)
		B_error(ERROR_ASM, "'add': memory to memory not supported.");

	ASM_DISPATCH_BINARY(dst, sub, sub);
}

void
asm_ret(void)
{
	emit_ret();
}

/******************************************************************************/

Offset
register_spill_offset(void)
{
	Scope	*current = arr_last(B.scopes);

	if (!current)
		B_error(ERROR_SYMBOL, "can't spill register without scope.");

	Offset	spill = -(current->stack + B.frame.spill);

	B.frame.spill += WORD_SIZE;
	
	return (spill);
}

Register
register_spill(Register wanted)
{
	RegState	*state = &B.frame.states[wanted];

	if (!state->spilled)
	{
		state->spill = register_spill_offset();
		state->in_use = false;
		state->spilled = true;

		Memory	tmp = MEM_STACK(state->spill);

		emit_mov_mem_reg(tmp, wanted);
	}
	return (wanted);
}

Register
register_restore(Register wanted)
{
	RegState	*state = &B.frame.states[wanted];

	if (state->spilled)
	{
		Memory	mem = MEM_STACK(state->spill);

		emit_mov_reg_mem(wanted, mem);

		state->spilled = false;
	}
}

Register
register_alloc(Register wanted)
{
	if (wanted > REG_USABLE)
		B_error(ERROR_ASM, "cannot reserve register '%s'", register_names[wanted]);

	if (wanted != REG_NULL)
	{
		RegState	*state = &B.frame.states[wanted];

		if (state->in_use)
			register_spill(wanted);

		state->in_use = true;
		BLOG("allocated register [%s]", register_names[wanted]);
		return (wanted);
	}

	for (Register reg = REG_NULL + 1; reg <= REG_USABLE; ++reg)
	{
		RegState	*state = &B.frame.states[reg];

		if (!state->in_use)
		{
			state->in_use = true;
			BLOG("allocated register [%s]", register_names[reg]);
			return (reg);
		}
	}
	BLOG("allocated spill slot.");
	return (register_spill(REG_NULL));
}

void
register_free(Register reg)
{
	if (reg > REG_USABLE)
		B_error(ERROR_ASM, "cannot free register '%s'", register_names[reg]);

	RegState	*state = &B.frame.states[reg];

	if (!state->in_use)
		return ;

	state->in_use = false;
	BLOG("freed register [%s]", register_names[reg]);
}

void
code_move(Expression dst, Expression src)
{
	if (dst.type == EXPR_MEMORY && src.type == EXPR_MEMORY)
	{
		Register	tmp = register_alloc(REG_NULL);

		asm_mov(REG(tmp), src);
		asm_mov(dst, REG(tmp));

		register_free(tmp);
	}
	else
		asm_mov(dst, src);
}

void
code_jump(LabelType type, StringC lbl)
{
	if (lbl)
	{
		emit_jmp_label(lbl);
		return ;
	}
	if (type == LABEL_NULL || type >= LABEL_ENUM_MAX)
		B_error(ERROR_SYNTAX, "invalid label type.");

	Label	label = B_label_get(type);

	emit_jmp_label(label.name);
}

void
code_label(LabelType type)
{
	if (type == LABEL_NULL || type >= LABEL_ENUM_MAX)
		B_error(ERROR_SYNTAX, "invalid label type.");

	Label	label = B_label_get(type);

	emit_label(label.name);
}

void
code_binop(BinopType type, Expression dst, Expression a, Expression b)
{
	code_move(dst, a);
	switch (type)
	{
		case BINOP_PLUS:
			asm_add(dst, b);
			break ;
		default:
			B_error(ERROR_SYNTAX, "invalid binop type.");
	}
	if (a.type == EXPR_REGISTER)
		register_free(a.reg);
	if (b.type == EXPR_REGISTER)
		register_free(b.reg);
}
