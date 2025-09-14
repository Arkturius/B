/**
 * codegen.h
 */

#if !defined (_CODEGEN_H)
# define _CODEGEN_H

# include <b.h>

typedef enum _bsyntax_type
{
	SYNTAX_INTEL,

	SYNTAX_ENUM_MAX,
}	SyntaxType;

typedef enum _bdirective_type
{
	DIRECTIVE_SYNTAX,
	DIRECTIVE_ALIGN,
	DIRECTIVE_SECTION,
	DIRECTIVE_GLOBAL,
	DIRECTIVE_LONG,

	DIRECTIVE_ENUM_MAX,
}	DirectiveType;

typedef enum _bsection_type
{
	SECTION_TEXT,
	SECTION_DATA,
	SECTION_RODATA,
	SECTION_BSS,

	SECTION_ENUM_MAX,
}	SectionType;

extern StringC	syntax_names[SYNTAX_ENUM_MAX];
# define		SYNTAX(_d)	syntax_names[(_d)]

extern StringC	directive_names[DIRECTIVE_ENUM_MAX];
# define		DIRECTIVE(_d)	directive_names[(_d)]

extern StringC	section_names[SECTION_ENUM_MAX];
# define		SECTION(_s)		section_names[(_s)]

struct _emit_directive_opt
{
	union
	{
		const u64	value;
		const void	*data;
	};
	const u32	off;
};

void
emit_directive_opt(DirectiveType type, struct _emit_directive_opt opt);

# define	emit_directive(_type, ...)	\
	emit_directive_opt(_type, (struct _emit_directive_opt){ .off = 5, __VA_ARGS__ })

/******************************************************************************/

void
emit_label(StringC label);


void
emit_push_imm(Immediate imm);

void
emit_push_reg(Register reg);

void
emit_push_mem(Memory mem);

void
emit_pop_imm(Immediate imm);

void
emit_pop_reg(Register reg);

void
emit_pop_mem(Memory mem);

# define	ASM_BINARY_OP_DECL(_instr)										\
																			\
void																		\
emit_##_instr##_reg_imm(Register dst, Immediate src);						\
																			\
void																		\
emit_##_instr##_reg_reg(Register dst, Register src);						\
																			\
void																		\
emit_##_instr##_reg_mem(Register dst, Memory src);							\
																			\
void																		\
emit_##_instr##_mem_imm(Memory dst, Immediate src);							\
																			\
void																		\
emit_##_instr##_mem_reg(Memory dst, Register src);							\

# define	ASM_BINARY_OP_IMPL(_instr)										\
																			\
void																		\
emit_##_instr##_reg_imm(Register dst, Immediate src)						\
{																			\
	EMIT(#_instr, "%s, %d", register_names[dst], src);						\
}																			\
																			\
void																		\
emit_##_instr##_reg_reg(Register dst, Register src)							\
{																			\
	EMIT(#_instr, "%s, %s", register_names[dst], register_names[src]);		\
}																			\
																			\
void																		\
emit_##_instr##_reg_mem(Register dst, Memory src)							\
{																			\
	StringC	mem = emit_mem(src, false);										\
																			\
	EMIT(#_instr, "%s, %s", register_names[dst], mem);						\
}																			\
																			\
void																		\
emit_##_instr##_reg_sym(Register dst, StringC src)							\
{																			\
	EMIT(#_instr, "%s, %s", register_names[dst], src);						\
}																			\
																			\
void																		\
emit_##_instr##_mem_imm(Memory dst, Immediate src)							\
{																			\
	StringC	mem = emit_mem(dst, true);										\
																			\
	EMIT(#_instr, "%s, %d", mem, src);										\
}																			\
																			\
void																		\
emit_##_instr##_mem_reg(Memory dst, Register src)							\
{																			\
	StringC	mem = emit_mem(dst, true);										\
																			\
	EMIT(#_instr, "%s, %s", mem, register_names[src]);						\
}																			\
																			\
void																		\
emit_##_instr##_mem_sym(Memory dst, StringC src)							\
{																			\
	StringC	mem = emit_mem(dst, true);										\
																			\
	EMIT(#_instr, "%s, %s", mem, src);										\
}																			\

ASM_BINARY_OP_DECL(mov)
ASM_BINARY_OP_DECL(add)
ASM_BINARY_OP_DECL(sub)

void
emit_ret_imm(Immediate imm);

void
emit_ret_reg(Register reg);

void
emit_ret_mem(Memory imm);


/******************************************************************************/

void
asm_push(Expression e);

void
asm_pop(Expression e);


void
asm_mov(Expression dst, Expression src);

void
asm_movzx(Register dst, Memory src);


void
asm_add(Expression dst, Expression add);

void
asm_sub(Expression dst, Expression sub);


void
asm_ret(void);

/******************************************************************************/

void
code_move(Expression dst, Expression src);

void
code_jump(CompareType cond, LabelType type, StringC lbl);

void
code_label(LabelType type);

void
code_binop(BinopType type, Expression dst, Expression a, Expression b);

void
code_call(Expression func);

void
code_compare(CompareType type, Expression a, Expression b);

Register
register_alloc(Register wanted);

void
register_free(Register reg);

void
register_restore(Register wanted);

#endif // _CODEGEN_H
