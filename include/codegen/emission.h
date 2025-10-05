/**
 * emission.h
 */

#if !defined (_EMISSION_H)
# define _EMISSION_H

# include <codegen/codegen.h>

typedef i32		x86Immediate;

/**
 * x86 Register bases.
 */
x_enum 
(
	x86RegisterBase,
	x_enum_prefix(X86_BASE),
	x_enum_members
	(
		(A, 1),
		(B   ),
		(C   ),
		(D   ),
		(BP  ),
		(SP  ),
		(DI  ),
		(SI  ),
	)
)

/**
 * x86 Register sizes.
 */
x_enum
(
	x86RegisterSize,
	x_enum_prefix(X86_SIZE),
	x_enum_members
	(
		(BYTE, 1),
		(WORD   ),
		(DWORD  ),
	)
)

# define	X86_REG_SIZE(_reg)	((_reg) & 0x3)
# define	X86_REG_BASE(_reg)	(((_reg) >> 2))

# define	X86_REG_MAKE(_base, _size)	(((_base << 2) | (_size)) & 0xff)

/**
 * x86 Registers.
 */
x_enum
(
	x86Register,
	x_enum_prefix(X86_REG),
	x_enum_members
	(
		(AL,  X86_REG_MAKE(X86_BASE_A,  X86_SIZE_BYTE)),
		(BL,  X86_REG_MAKE(X86_BASE_B,  X86_SIZE_BYTE)),
		(CL,  X86_REG_MAKE(X86_BASE_C,  X86_SIZE_BYTE)),
		(DL,  X86_REG_MAKE(X86_BASE_D,  X86_SIZE_BYTE)),

		(AX,  X86_REG_MAKE(X86_BASE_A,  X86_SIZE_WORD)),
		(BX,  X86_REG_MAKE(X86_BASE_B,  X86_SIZE_WORD)),
		(CX,  X86_REG_MAKE(X86_BASE_C,  X86_SIZE_WORD)),
		(DX,  X86_REG_MAKE(X86_BASE_D,  X86_SIZE_WORD)),
		(BP,  X86_REG_MAKE(X86_BASE_BP, X86_SIZE_WORD)),
		(SP,  X86_REG_MAKE(X86_BASE_SP, X86_SIZE_WORD)),
		(DI,  X86_REG_MAKE(X86_BASE_DI, X86_SIZE_WORD)),
		(SI,  X86_REG_MAKE(X86_BASE_SI, X86_SIZE_WORD)),

		(EAX, X86_REG_MAKE(X86_BASE_A,  X86_SIZE_DWORD)),
		(EBX, X86_REG_MAKE(X86_BASE_B,  X86_SIZE_DWORD)),
		(ECX, X86_REG_MAKE(X86_BASE_C,  X86_SIZE_DWORD)),
		(EDX, X86_REG_MAKE(X86_BASE_D,  X86_SIZE_DWORD)),
		(EBP, X86_REG_MAKE(X86_BASE_BP, X86_SIZE_DWORD)),
		(ESP, X86_REG_MAKE(X86_BASE_SP, X86_SIZE_DWORD)),
		(EDI, X86_REG_MAKE(X86_BASE_DI, X86_SIZE_DWORD)),
		(ESI, X86_REG_MAKE(X86_BASE_SI, X86_SIZE_DWORD)),
	)
)
extern StringC	ASM_register_names[X86_BASE_XENUM_LAST][X86_SIZE_XENUM_LAST];
# define		ASM_register_name(_r)	ASM_register_names[X86_REG_BASE(_r)][X86_REG_SIZE(_r)]


/**
 * @brief	x86 memory scale modes.
 */
x_enum
(
	x86MemoryScale,
	x_enum_prefix(X86_MEM_SCALE),
	x_enum_members
	(
		(ARCH,  (0)),
		(BYTE,  (1 << 0)),
		(WORD,  (1 << 1)),
		(DWORD, (1 << 2)),
	)
)

x_enum 
(
	x86OperandType,
	x_enum_prefix(OPERAND),
	x_enum_members
	(
		(IMMEDIATE),
		(REGISTER ),
		(MEMORY   ),
		(SYMBOL   ),
	)
)

typedef struct _x86_memory
{
	x86Register		base;
	x86Register		index;
	x86MemoryScale	scale;
	x86Immediate	displacement;
	x86MemoryScale	size;
}	x86Memory;

# define	X86_MEM(...)	(x86Memory) { .size = X86_MEM_SCALE_ARCH, ##__VA_ARGS__ }

typedef StringC	x86Symbol;

typedef struct	_x86_operand
{
	x86OperandType	type;
	union 
	{
		x86Register		reg;
		x86Immediate	imm;
		x86Memory		mem;
		x86Symbol		sym;
	};
}	x86Operand;

# define	IMM_OPERAND(_i)	(x86Operand){ .type = OPERAND_IMMEDIATE, .imm = (_i) }
# define	REG_OPERAND(_r)	(x86Operand){ .type = OPERAND_REGISTER,  .reg = (_r) }
# define	MEM_OPERAND(_m)	(x86Operand){ .type = OPERAND_MEMORY,    .mem = (_m) }
# define	SYM_OPERAND(_s)	(x86Operand){ .type = OPERAND_SYMBOL,    .sym = (_s) }

/**
 * @brief	x86 instruction set. both enum and emission functions.
 */

# define	ASM_instr(_instr)		concat(INSTRUCTION_, _instr)
# define	ASM_expand(...)			__VA_ARGS__
# define	ASM_expand2(...)		__VA_ARGS__

# define	ASM_gen_decl_0(_name)											\
																			\
	_unused static void concat(ASM_, _name)(void)

# define	ASM_gen_decl_1(_name)											\
																			\
	_unused static void concat(ASM_, _name)(x86Operand a)

# define	ASM_gen_decl_2(_name)											\
																			\
	_unused static void concat(ASM_, _name)(x86Operand a, x86Operand b)


# define	ASM_gen_code_0(_enum, _instr)									\
																			\
	{ concat(ASM_emit_, _enum)(ASM_instr(_instr), NULL, NULL); }

# define	ASM_gen_code_1(_enum, _instr)									\
																			\
	{ concat(ASM_emit_, _enum)(ASM_instr(_instr), &a, NULL);   }

# define	ASM_gen_code_2(_enum, _instr)									\
																			\
	{ concat(ASM_emit_, _enum)(ASM_instr(_instr), &a, &b);     }

# define	ASM_gen_decl(_n, _name)											\
																			\
	concat(ASM_gen_decl_, _n)(_name)

# define	ASM_gen_code(_enum, _n, _instr)									\
																			\
	concat(ASM_gen_code_, _n)(_enum, _instr)

# define	ASM_gen_instr_wrap(_n, _instr, _name)							\
																			\
	(_instr),

# define	ASM_gen_func_wrap(_enum, _n, _instr, _name)						\
																			\
	ASM_gen_decl(_n, _name)													\
	ASM_gen_code(_enum, _n, _instr)

# define	ASM_gen_func(_enum, _args)										\
																			\
	ASM_expand2(ASM_gen_func_wrap x_lparen _enum, ASM_expand _args x_rparen)

# define	ASM_gen_instr(_args)											\
																			\
	ASM_gen_instr_wrap _args

# define	ASM_instruction_set(_enum, ...)									\
																			\
	x_enum																	\
	(																		\
		_enum,																\
		x_enum_prefix(INSTRUCTION),											\
		x_foreach(ASM_gen_instr, __VA_ARGS__)								\
	)																		\
	void																	\
	concat(ASM_emit_, _enum)(_enum instr, x86Operand *a, x86Operand *b);	\
	x_foreach_1(ASM_gen_func, _enum, __VA_ARGS__)

ASM_instruction_set
(
	x86Instruction,
	(2, MOV  , mov  ),
 	(2, MOVZX, movzx),
	(2, LEA  , lea  ),
	(2, ADD  , add  ),
	(2, SUB  , sub  ),
	(2, AND  , and  ),
	(2, OR   , or   ),
	(2, XOR  , xor  ),
	(2, SHR  , shr  ),
	(2, SHL  , shl  ),
	(2, IDIV , idiv ),
	(2, IMUL , imul ),
	(2, CMP  , cmp  ),
	(2, TEST , test ),
	(1, NOT  , not  ),
	(1, PUSH , push ),
	(1, POP  , pop  ),
	(1, CALL , call ),
  	(0, RET  , ret  ),
  	(0, CDQ  , cdq  ),
)

extern StringC	ASM_instruction_names[INSTRUCTION_XENUM_LAST];
# define		ASM_instruction_name(_e)	ASM_instruction_names[(_e)]

/**
 * @brief	assembly section types.
 */
x_enum
(
	SectionType, 
	x_enum_prefix(SECTION),
	x_enum_members
	(
		(NONE  ),
		(TEXT  ),
		(DATA  ),
		(RODATA),
		(BSS   ),
	)
)
extern StringC	ASM_section_names[SECTION_XENUM_LAST];
# define		ASM_section_name(_e)	ASM_section_names[(_e)]

/**
 * @brief	GNU 'as' directive types.
 */
x_enum 
(
	DirectiveType,
	x_enum_prefix(DIRECTIVE),
	x_enum_members
	(
		(SYNTAX	),
		(ALIGN	),
		(SECTION),
		(GLOBAL	),
		(LONG	),
		(STRING	),
	)
);
extern StringC	ASM_directive_names[DIRECTIVE_XENUM_LAST];
# define		ASM_directive_name(_e)	ASM_directive_names[(_e)]

typedef struct
{
	StringC	str;
	void	*data;
}	DirectiveOpt;

# define	ASM_directive(_t, ...)											\
			ASM_emit_directive(_t, (DirectiveOpt){__VA_ARGS__})

# define	ASM_dir_syntax()												\
			ASM_directive(DIRECTIVE_SYNTAX, .str = "noprefix")

# define	ASM_dir_align(_d)												\
			ASM_directive(DIRECTIVE_ALIGN, .data = (void *)(long)(_d))

# define	ASM_dir_section(_s)												\
			ASM_directive(DIRECTIVE_SECTION, .str = ASM_section_name(_s))

# define	ASM_dir_global(_d)												\
			ASM_directive(DIRECTIVE_GLOBAL, .str  = (_d))

# define	ASM_dir_long(_s, _d)											\
			ASM_directive(DIRECTIVE_LONG, .str = (_s), .data = (void *)(_d))

# define	ASM_dir_string(_d)												\
			ASM_directive(DIRECTIVE_STRING, .str  = (_d))

void
ASM_emit_directive(DirectiveType t, DirectiveOpt opt);

void
ASM_emit_operand_immediate(x86Immediate imm);

void
ASM_emit_operand_register(x86Register reg);

void
ASM_emit_operand_memory(x86Memory mem, bool destination);

void
ASM_emit_operand_symbol(x86Symbol sym);

void
ASM_emit_symbol(x86Symbol sym);

void
ASM_label(StringC label);

#endif // _EMISSION_H
