/**
 * codegen.c
 */

#include "expression.h"
#include <assert.h>
#include <b.h>
#include <codegen.h>
#include <stdbool.h>

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

StringC	register_bytes[REG_ENUM_MAX] = 
{
	[REG_EDX] = "dl",
	[REG_EAX] = "al",
	[REG_ECX] = "cl",
	[REG_EBX] = "bl",
	[REG_EDI] = "edi",
	[REG_ESI] = "esi",
	[REG_ESP] = "esp",
	[REG_EBP] = "ebp",
};

StringC jump_ccs[BINOP_COMP_ENUM_MAX] =
{
	[BINOP]		= "jmp",
	[BINOP_EQ]	= "je",
	[BINOP_NE]	= "jne",
	[BINOP_GT]	= "jg",
	[BINOP_GE]	= "jge",
	[BINOP_LT]	= "jl",
	[BINOP_LE]	= "jle",
};

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

void
emit_idiv_reg(Register reg)
{
	EMIT("idiv", "%s", register_names[reg]);
}

void
emit_idiv_mem(Memory mem)
{
	StringC	div = emit_mem(mem, true);

	EMIT("idiv", "%s", div);
}

ASM_BINARY_OP_IMPL(mov);
ASM_BINARY_OP_IMPL(add);
ASM_BINARY_OP_IMPL(sub);
ASM_BINARY_OP_IMPL(and);
ASM_BINARY_OP_IMPL(xor);

ASM_BINARY_OP_IMPL(cmp);
ASM_BINARY_OP_IMPL(test);

void
emit_ret()
{
	EMIT("ret", "");
}

void
emit_cdq()
{
	EMIT("cdq", "");
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

	if (src.type == EXPR_IMMEDIATE && src.imm == 0 && dst.type == EXPR_REGISTER)
	{
		emit_xor_reg_reg(dst.reg, dst.reg);
		return ;
	}

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
asm_div(Expression div)
{
	if (div.type != EXPR_MEMORY && div.type != EXPR_REGISTER)
		B_error(ERROR_ASM, "'idiv': operand not supported.");

	emit_cdq();
	switch (div.type)
	{
		case EXPR_REGISTER:
			emit_idiv_reg(div.reg);
			break ;
		case EXPR_MEMORY:
			emit_idiv_mem(div.mem);
			break ;
		default:
			B_error(ERROR_ASM, "'idiv': invalid operand type.");
	}
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
asm_jump(BinopType type, StringC lbl)
{
	if (type >= BINOP_COMP_ENUM_MAX)
		B_error(ERROR_ASM, "invalid jump condition type.");

	emit_jmp(JCC(type), lbl);
}

void
asm_load(Register dst, Memory src)
{
	emit_lea(dst, src);
}

/******************************************************************************/

/****************************************************************************/

Expression
code_register(Register reg)
{
	RegState	*states;

	if (reg != REG_NULL)
		BTODO("dedicated register allocation.");

	states = B.frame.states;
	for (Register r = 0; r < REG_USABLE; ++r)
	{
		if (states[r].in_use)
			continue;
		states[r].in_use = true;
		return (REG(r));
	}
	B_error(ERROR_ASM, "cannot allocate register.");
}

void
code_register_release(Expression e)
{
	if (e.type != EXPR_REGISTER)
		return ;
	
	RegState	*state = &B.frame.states[e.reg];

	if (!state->in_use)
		return ;
	state->in_use = false;
}

void
code_register_cleanup(void)
{
	for (Register r = 0; r < REG_USABLE; ++r)
		code_register_release(REG(r));
}

static bool
code_expression_cmp(Expression *a, Expression *b)
{
	return (memcmp(a, b, sizeof(Expression)) == 0);
}

static void
code_operands_binary(Expression *dst, Expression *src)
{
	if (code_expression_cmp(dst, src))
		return ;
	
	if (dst->type == EXPR_MEMORY && (src->type == EXPR_MEMORY || src->type == EXPR_SYMBOL))
	{
		Expression	temp = code_register(REG_NULL);

		code_move(temp, *src);
		*src = temp;
	}
}

Expression
code_move(Expression dst, Expression src)
{
	code_operands_binary(&dst, &src);

	asm_mov(dst, src);

	if (src.type == EXPR_REGISTER && src.reg <= REG_USABLE)
		register_free(src.reg);

	return (dst);
}

void
code_jump(BinopType cond, LabelType type, StringC lbl)
{
	StringC	jump;

	if (lbl)
		jump = lbl;
	else
	{
		if (type == LABEL_NULL || type >= LABEL_ENUM_MAX)
			B_error(ERROR_SYNTAX, "invalid label type.");

		Label	label = B_label_get(type);

		jump = label.name;
	}
	asm_jump(cond, jump);
}

void
code_label(LabelType type)
{
	if (type == LABEL_NULL || type >= LABEL_ENUM_MAX)
		B_error(ERROR_SYNTAX, "invalid label type.");

	Label	label = B_label_get(type);

	asm_label(label.name);
}
/****************************************************************************/

void
code_binop(BinopType type, Expression dst, Expression a, Expression b)
{
	if (type >= BINOP_LSHIFT)
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

		case BINOP_DIV:
		case BINOP_MOD:
		{
			Register	divisor;

			if (b.type != EXPR_REGISTER && b.type != EXPR_MEMORY)
			{
				divisor = register_alloc(REG_NULL);

				code_move(REG(divisor), b);
				b = REG(divisor);
			}
			asm_div(b);
			if (type == BINOP_MOD)
				code_move(EAX, EDX);

			break ;	
		}

		case BINOP_EQ:
		case BINOP_GT:
		case BINOP_GE:
		case BINOP_LT:
		case BINOP_LE:
		{
			if (a.type == EXPR_MEMORY && b.type == EXPR_MEMORY)
			{
				Register	tmp = register_alloc(REG_NULL);

				code_move(REG(tmp), a);
				a = REG(tmp);
			}
			asm_cmp(a, b);
			break ;
		}
		default:
			B_error(ERROR_SYNTAX, "invalid binop type. %d", type);
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
	}
	else
		asm_call(func);
}

Expression
code_load(Expression src)
{
	Expression	dst;

	dst = src;
	if (src.type != EXPR_REGISTER)
	{
		dst = code_register(REG_NULL);
		dst = code_move(dst, src);
	}
	asm_load(dst.reg, src.mem);

	return (dst);
}

