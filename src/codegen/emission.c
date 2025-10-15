/**
 * emission.c
 */

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <b.h>
#include <eval/declaration.h>
#include <codegen/emission.h>

# define	X86_INSTR_NAME(_enum, _op, _name)	[concat(INSTRUCTION, _op)] = stringify(_name),

StringC	ASM_instruction_names[INSTRUCTION_XENUM_LAST] =
{
	[INSTRUCTION_MOV   ] = "mov",
	[INSTRUCTION_MOVZX ] = "movzx",
	[INSTRUCTION_CMOVE ] = "cmove",
	[INSTRUCTION_CMOVNE] = "cmovne",
	[INSTRUCTION_LEA   ] = "lea",
	[INSTRUCTION_ADD   ] = "add",
	[INSTRUCTION_SUB   ] = "sub",
	[INSTRUCTION_AND   ] = "and",
	[INSTRUCTION_OR    ] = "or",
	[INSTRUCTION_XOR   ] = "xor",
	[INSTRUCTION_SHR   ] = "shr",
	[INSTRUCTION_SHL   ] = "shl",
	[INSTRUCTION_IDIV  ] = "idiv",
	[INSTRUCTION_IMUL  ] = "imul",
	[INSTRUCTION_NEG   ] = "neg",
	[INSTRUCTION_NOT   ] = "not",
	[INSTRUCTION_CMP   ] = "cmp",
	[INSTRUCTION_TEST  ] = "test",
	[INSTRUCTION_PUSH  ] = "push",
	[INSTRUCTION_POP   ] = "pop",
	[INSTRUCTION_JMP   ] = "jmp",
	[INSTRUCTION_JE    ] = "je",
	[INSTRUCTION_JNE   ] = "jne",
	[INSTRUCTION_JG    ] = "jg",
	[INSTRUCTION_JL    ] = "jl",
	[INSTRUCTION_JGE   ] = "jge",
	[INSTRUCTION_JLE   ] = "jle",
	[INSTRUCTION_JA    ] = "ja",
	[INSTRUCTION_SETE  ] = "sete",
	[INSTRUCTION_SETNE ] = "setne",
	[INSTRUCTION_SETG  ] = "setg",
	[INSTRUCTION_SETL  ] = "setl",
	[INSTRUCTION_SETGE ] = "setge",
	[INSTRUCTION_SETLE ] = "setle",
	[INSTRUCTION_CALL  ] = "call",
	[INSTRUCTION_RET   ] = "ret",
	[INSTRUCTION_CDQ   ] = "cdq",
};

StringC	ASM_directive_names[DIRECTIVE_XENUM_LAST] =
{
	[DIRECTIVE_SYNTAX ] = ".intel_syntax",
	[DIRECTIVE_ALIGN  ] = ".align",
	[DIRECTIVE_PALIGN ] = ".p2align",
	[DIRECTIVE_SECTION] = ".section",
	[DIRECTIVE_GLOBAL ] = ".globl",
	[DIRECTIVE_LONG   ] = ".long",
	[DIRECTIVE_STRING ] = ".string",

#if defined(B_DEBUG)
	[DIRECTIVE_FILE   ] = ".file",
	[DIRECTIVE_LOC    ] = ".loc",
	[DIRECTIVE_TYPE   ] = ".type",
	[DIRECTIVE_SIZE   ] = ".size",
	[DIRECTIVE_CFI_SP ] = ".cfi_startproc",
	[DIRECTIVE_CFI_EP ] = ".cfi_endproc",
#endif

};

StringC	ASM_section_names[SECTION_XENUM_LAST] =
{
	[SECTION_TEXT  ] = ".text",
	[SECTION_DATA  ] = ".data",
	[SECTION_RODATA] = ".rodata",
	[SECTION_BSS   ] = ".bss",
};

StringC	ASM_register_names[X86_BASE_XENUM_LAST][X86_SIZE_XENUM_LAST] = 
{
	{NULL, NULL, NULL, NULL },
	{NULL, "al", "ax", "eax"},
	{NULL, "bl", "bx", "ebx"},
	{NULL, "cl", "cx", "ecx"},
	{NULL, "dl", "dx", "edx"},
	{NULL, NULL, "bp", "ebp"},
	{NULL, NULL, "sp", "esp"},
	{NULL, NULL, "di", "edi"},
	{NULL, NULL, "si", "esi"},
};

static inline bool
ASM_is_reg(x86Register reg)
{
	if (reg >= X86_REG_XENUM_LAST)
		return (false);
	return (ASM_register_name(reg) != NULL);
}

static inline bool
ASM_is_scale(x86MemoryScale scale)
{
	switch (scale)
	{
		case X86_MEM_SCALE_ARCH:
		case X86_MEM_SCALE_BYTE:
		case X86_MEM_SCALE_WORD:
		case X86_MEM_SCALE_DWORD:
			return (true);
		default:
			return (false);
	}
}

static inline bool
ASM_is_size(x86RegisterSize size)
{
	switch (size)
	{
		case X86_SIZE_ARCH:
		case X86_SIZE_BYTE:
		case X86_SIZE_WORD:
		case X86_SIZE_DWORD:
			return (true);
		default:
			return (false);
	}
}

void
ASM_label(StringC label)
{
	B_DBG_TREE;

	printf("%s:\n", label);
}

void
ASM_emit_operand_immediate(x86Immediate imm)
{
	B_DBG_TREE;

	printf("0x%02x", imm);
}

void
ASM_emit_operand_register(x86Register reg)
{
	B_DBG_TREE;

	StringC	name = ASM_register_name(reg);

	if (!name)
		unreachable("invalid x86Register.");

	printf("%s", name);
}

static void
ASM_emit_operand_memory_size(x86RegisterSize size, bool destination)
{
	B_DBG_TREE;

	assert(ASM_is_size(size) && "invalid x86Memory size.");

	if (destination)
	{
		switch (size)
		{
			case X86_SIZE_BYTE:
				printf("BYTE PTR ");
				break ;
			case X86_SIZE_WORD:
				printf("WORD PTR ");
				break ;
			case X86_SIZE_ARCH:					  
			case X86_SIZE_DWORD:
				printf("DWORD PTR ");
				break ;
			default:
				break ;
		}
	}
}

void
ASM_emit_operand_memory(x86Memory mem, bool destination)
{
	B_DBG_TREE;

	int	before = 0;

	ASM_emit_operand_memory_size(mem.size, destination);

	printf("[");
	if (ASM_is_reg(mem.base) && !before++)
		ASM_emit_operand_register(mem.base);

	if (ASM_is_reg(mem.index))
	{
		if (before && before++)
			printf(" + ");
		ASM_emit_operand_register(mem.index);

		if (mem.scale && ASM_is_scale(mem.scale))
		{
			printf(" * ");
			ASM_emit_operand_immediate(mem.scale);
		}
	}
	if (mem.displacement)
	{
		if (mem.displacement > 0)
		{
			if (before && before++)
				printf(" + ");
			ASM_emit_operand_immediate(mem.displacement);
		}
		else
		{
			if (before && before++)
				printf(" - ");
			ASM_emit_operand_immediate(-mem.displacement);
		}
	}
	printf("]");
}

void
ASM_emit_operand_symbol(x86Symbol sym)
{
	B_DBG_TREE;

	assert(sym != NULL && "invalid x86Symbol (null).");

	ASM_emit_operand_memory_size(X86_SIZE_DWORD, true);
	printf("[%s]", sym);
}

void
ASM_emit_symbol(x86Symbol sym)
{
	B_DBG_TREE;

	assert(sym != NULL && "invalid x86Symbol (null).");

	printf("%s", sym);
}

void
ASM_emit_operand(x86Operand op)
{
	B_DBG_TREE;

	if (op.type >= OPERAND_XENUM_LAST)
		exit(1);

	switch (op.type)
	{
		case OPERAND_REGISTER:
			ASM_emit_operand_register(op.reg);
			break ;
		case OPERAND_IMMEDIATE:
			ASM_emit_operand_immediate(op.imm);
			break ;
		case OPERAND_MEMORY:
			ASM_emit_operand_memory(op.mem, (op.mem.size != 0));
			break ;
		case OPERAND_SYMBOL:
			if (op.internal)
				ASM_emit_symbol(op.sym);
			else
				ASM_emit_operand_symbol(op.sym);
			break ;
		default:
			break ;
	}
}

static void
ASM_emit_long_list(void *data)
{
	B_DBG_TREE;

	Expressions	*ivals = data;

	if (arr_count(*ivals) == 0)
	{
		printf(" ");
		ASM_emit_operand(IMM_OPERAND(0));
		return ;
	}
	arr_foreach(Expression, ival, *ivals)
	{
		if (arr_index(*ivals, ival) > 0)
			printf(",");
		printf(" ");

		x86Operand	op = arr_nth(EA, *ival)->op;

		op.internal = true;
		ASM_emit_operand(op);
	}
}

void
ASM_emit_directive(DirectiveType t, DirectiveOpt opt)
{
	B_DBG_TREE;

	assert(t < DIRECTIVE_XENUM_LAST && "invalid DirectiveType");

	StringC	dir_str = ASM_directive_name(t);
	
	printf("    %s", dir_str);
	switch (t)
	{
		case DIRECTIVE_LONG:
		{
			if (opt.str)
			{
				printf(" %s", opt.str);
				if (opt.data)
					printf(" + %ld", (long int) opt.data);
			}
			else
				ASM_emit_long_list(opt.data);
			break ;
		}
		case DIRECTIVE_ALIGN:
			printf(" %ld, ", (long int) opt.data);
			break ;
		case DIRECTIVE_PALIGN:
			printf(" %ld, 0x90", (long int) opt.data);
			break ;
		case DIRECTIVE_STRING:
			printf(" \"%s\"", opt.str);
			break ;
#ifdef B_DEBUG
		case DIRECTIVE_FILE:
			printf(" 1 \"%s\" \"%s\"", B.directory, B.input_file);
			break ;
		case DIRECTIVE_LOC:
			printf(" 1 %d %d %s", yylineno, b_col, opt.str);
			break ;
		case DIRECTIVE_TYPE:
			printf(" %s, @function", opt.str);
			break ;
		case DIRECTIVE_CFI_SP:
		case DIRECTIVE_CFI_EP:
			break ;
		case DIRECTIVE_SIZE:
#endif
		default:
			printf(" %s", opt.str);
	}
	printf("\n");
}

void
ASM_emit_x86Instruction(x86Instruction instr, x86Operand *a, x86Operand *b)
{
	B_DBG_TREE;

	assert(instr < INSTRUCTION_XENUM_LAST && "invalid x86Instruction.");

	StringC	mnemonic = ASM_instruction_name(instr);

	printf("    %s%*.s", mnemonic, (int)(8 - strlen(mnemonic)), "");
	
	if (a)
		ASM_emit_operand(*a);
	if (b)
	{
		printf(", ");
		ASM_emit_operand(*b);
	}
	printf("\n");
}

