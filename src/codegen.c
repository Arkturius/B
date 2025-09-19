/**
 * codegen.c
 */

#include "expression.h"
#include <assert.h>
#include <b.h>
#include <codegen.h>
#include <stdbool.h>

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
	[DIRECTIVE_STRING]	= ".string",
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
			break ;
		case DIRECTIVE_STRING:
			printf("%s", (String)opt.data);
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
emit_push_sym(StringC sym)
{
	EMIT("push", "%s", sym);
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

void
emit_pop_sym(StringC sym)
{
	EMIT("pop", "%s", sym);
}

ASM_BINARY_OP_IMPL(mov);
ASM_BINARY_OP_IMPL(add);
ASM_BINARY_OP_IMPL(sub);
ASM_BINARY_OP_IMPL(and);

ASM_BINARY_OP_IMPL(cmp);
ASM_BINARY_OP_IMPL(test);

void
emit_ret()
{
	EMIT("ret", "");
}

void
emit_jmp(StringC jump, StringC label)
{
	EMIT(jump, "%s", label);
}

void
emit_call(StringC func)
{
	EMIT("call", "[%s]", func);
}

void
emit_lea(Register dst, Memory src)
{
	StringC	mem = emit_mem(src, false);

	EMIT("lea", "%s, %s", register_names[dst], mem);
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
		case EXPR_SYMBOL:
			emit_push_sym(e.sym);
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
		case EXPR_SYMBOL:
			emit_pop_sym(e.sym);
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
	OP_REG_SYM,
	OP_MEM_IMM,
	OP_MEM_REG,
	OP_INVALID,
	OP_MEM_SYM,
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
			case EXPR_SYMBOL:	 return (OP_REG_SYM);
			default:             break ;
		}
		break ;
		case EXPR_MEMORY:
		switch (src.type)
		{
			case EXPR_IMMEDIATE: return (OP_MEM_IMM);
			case EXPR_REGISTER:  return (OP_MEM_REG);
			case EXPR_MEMORY:	 return (OP_INVALID);
			case EXPR_SYMBOL:    return (OP_MEM_SYM);
			default:             break ;
		}
		break ;
		default:
			B_error(ERROR_ASM, "invalid operand type.");
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
			case OP_REG_SYM:												\
				emit_##_instr##_reg_sym(_a.reg, _b.sym);					\
				break ;														\
			case OP_MEM_IMM:												\
				emit_##_instr##_mem_imm(_a.mem, _b.imm);					\
				break ;														\
			case OP_MEM_REG:												\
				emit_##_instr##_mem_reg(_a.mem, _b.reg);					\
				break ;														\
			case OP_MEM_SYM:												\
				emit_##_instr##_mem_sym(_a.mem, _b.sym);					\
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
asm_movzx(Register dst, Memory src)
{
	StringC	from = emit_mem(src, true);

	EMIT("movzx", "%s, %s", register_names[dst], from);
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
asm_and(Expression a, Expression b)
{
	if (a.type == EXPR_MEMORY && b.type == EXPR_MEMORY)
		B_error(ERROR_ASM, "'and': memory to memory not supported.");

	ASM_DISPATCH_BINARY(a, b, and);
}

void
asm_test(Expression a, Expression b)
{
	if (a.type == EXPR_MEMORY && b.type == EXPR_MEMORY)
		B_error(ERROR_ASM, "'test': memory vs memory comparison not supported.");

	ASM_DISPATCH_BINARY(a, b, test);
}

void
asm_cmp(Expression a, Expression b)
{
	if (a.type == EXPR_MEMORY && b.type == EXPR_MEMORY)
		B_error(ERROR_ASM, "'cmp': memory vs memory comparison not supported.");

	ASM_DISPATCH_BINARY(a, b, cmp);
}

void
asm_call(Expression func)
{
	if (func.type == EXPR_REGISTER)
		emit_call(register_names[func.reg]);
	else
		emit_call(func.sym);
}

void
asm_ret(void)
{
	emit_ret();
}

void
asm_jump(CompareType type, StringC lbl)
{
	if (type >= COMP_ENUM_MAX)
		B_error(ERROR_ASM, "invalid jump condition type.");

	emit_jmp(JCC(type), lbl);
}

void
asm_load(Register dst, Memory src)
{
	emit_lea(dst, src);
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

void
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

bool
register_is_same(Expression a, Expression b)
{
	if (a.type != EXPR_REGISTER || b.type != EXPR_REGISTER)
		B_error(ERROR_ASM, "can't compare non-register expressions.'");

	return (a.reg == b.reg);
}

bool
memory_is_same(Expression a, Expression b)
{
	if (a.type != EXPR_MEMORY || b.type != EXPR_MEMORY)
		B_error(ERROR_ASM, "can't compare non-memory expressions.'");

	if (a.mem.base != b.mem.base)
		return (false);
	if (a.mem.index != b.mem.index)
		return (false);
	if (a.mem.scale != b.mem.scale)
		return (false);
	if (a.mem.displacement != b.mem.displacement)
		return (false);
	return (true);
}

void
code_move(Expression dst, Expression src)
{
	if (dst.type == EXPR_REGISTER && src.type == EXPR_REGISTER && register_is_same(dst, src))
		return ;
	if (dst.type == EXPR_MEMORY && src.type == EXPR_MEMORY && memory_is_same(dst, src))
		return ;

	if (dst.type == EXPR_MEMORY && (src.type == EXPR_MEMORY || src.type == EXPR_SYMBOL))
	{
		Register	tmp = register_alloc(REG_NULL);

		asm_mov(REG(tmp), src);
		asm_mov(dst, REG(tmp));

		register_free(tmp);
	}
	else
		asm_mov(dst, src);
	if (src.type == EXPR_REGISTER && src.reg <= REG_USABLE)
		register_free(src.reg);
}

void
code_jump(CompareType cond, LabelType type, StringC lbl)
{
	if (lbl)
	{
		asm_jump(cond, lbl);
		return ;
	}
	if (type == LABEL_NULL || type >= LABEL_ENUM_MAX)
		B_error(ERROR_SYNTAX, "invalid label type.");

	Label	label = B_label_get(type);

	asm_jump(cond, label.name);
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
		case BINOP_MINUS:
			asm_sub(dst, b);
			break ;
		case BINOP_AND:
			asm_and(dst, b);
			break ;
		default:
			B_error(ERROR_SYNTAX, "invalid binop type.");
	}
}

void
code_call(Expression func)
{
	Register	tmp = func.reg;

	if (func.type == EXPR_MEMORY)
	{
		tmp = register_alloc(REG_NULL);
		asm_mov(REG(tmp), func);
		asm_call(REG(tmp));
		register_free(tmp);
	}
	else
		asm_call(func);
}

void
code_compare(CompareType type, Expression a, Expression b)
{
	switch (type)
	{
		case COMP_E:
		case COMP_NE:
			asm_test(a, b);
			break ;
		default:
			BTODO("more CompareType s.");
	}
}

void
code_load(Expression dst, Expression src)
{
//	assert(dst.type == EXPR_REGISTER && src.type == EXPR_MEMORY);

	asm_load(dst.reg, src.mem);
}

void
code_deref(Expression dst, Expression src)
{
	asm_mov(dst, src);
}
