/**
 * emission.c
 */

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <eval/declaration.h>
#include <codegen/emission.h>

# define	X86_INSTR_NAME(_enum, _op, _name)	[concat(INSTRUCTION, _op)] = stringify(_name),

StringC	ASM_instruction_names[INSTRUCTION_XENUM_LAST] =
{
	[INSTRUCTION_MOV  ] = "mov",
	[INSTRUCTION_MOVZX] = "movz",
	[INSTRUCTION_LEA  ] = "lea",
	[INSTRUCTION_ADD  ] = "add",
	[INSTRUCTION_SUB  ] = "sub",
	[INSTRUCTION_AND  ] = "and",
	[INSTRUCTION_OR   ] = "or",
	[INSTRUCTION_XOR  ] = "xor",
	[INSTRUCTION_SHR  ] = "shr",
	[INSTRUCTION_SHL  ] = "shl",
	[INSTRUCTION_IDIV ] = "idiv",
	[INSTRUCTION_IMUL ] = "imul",
	[INSTRUCTION_NOT  ] = "not",
	[INSTRUCTION_CMP  ] = "cmp",
	[INSTRUCTION_TEST ] = "test",
	[INSTRUCTION_PUSH ] = "push",
	[INSTRUCTION_POP  ] = "pop",
	[INSTRUCTION_CALL ] = "call",
	[INSTRUCTION_RET  ] = "ret",
	[INSTRUCTION_CDQ  ] = "cdq",
};

StringC	ASM_directive_names[DIRECTIVE_XENUM_LAST] =
{
	[DIRECTIVE_SYNTAX ] = ".intel_syntax",
	[DIRECTIVE_ALIGN  ] = ".align",
	[DIRECTIVE_SECTION] = ".section",
	[DIRECTIVE_GLOBAL ] = ".global",
	[DIRECTIVE_LONG   ] = ".long",
	[DIRECTIVE_STRING ] = ".string",
};

StringC	ASM_section_names[SECTION_XENUM_LAST] =
{
	[SECTION_TEXT]   = ".text",
	[SECTION_DATA]   = ".data",
	[SECTION_RODATA] = ".rodata",
	[SECTION_BSS]    = ".bss",
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
		case X86_MEM_SCALE_BYTE:
		case X86_MEM_SCALE_WORD:
		case X86_MEM_SCALE_DWORD:
			return (true);
		default:
			return (false);
	}
}

static void
ASM_emit_long_list(void *data)
{
	IVals	*ivals = data;

	arr_foreach(IVal, ival, *ivals)
	{
		if (arr_index(*ivals, ival) > 0)
			printf(",");
		printf(" ");
		if (ival->sym)
			ASM_emit_symbol(ival->sym);
		else
			ASM_emit_operand_immediate(ival->imm);
	}
}

void
ASM_emit_directive(DirectiveType t, DirectiveOpt opt)
{
	assert(t < DIRECTIVE_XENUM_LAST && "invalid DirectiveType");

	StringC	dir_str = ASM_directive_name(t);
	
	printf("%s", dir_str);
	switch (t)
	{
		case DIRECTIVE_LONG:
		{
			if (opt.str)
				printf(" %s + %ld", opt.str, (long int) opt.data);
			else
				ASM_emit_long_list(opt.data);
			break ;
		}
		case DIRECTIVE_ALIGN:
			printf(" %ld", (long int) opt.data);
			break ;
		default:
			printf(" %s", opt.str);
			break ;
	}
	printf("\n");
}

void
ASM_label(StringC label)
{
	printf("%s:\n", label);
}

void
ASM_emit_operand_immediate(x86Immediate imm)
{
	printf("0x%02x", imm);
}

void
ASM_emit_operand_register(x86Register reg)
{
	StringC	name = ASM_register_name(reg);

	assert(name && "invalid x86Register.");

	printf("%s", name);
}

static void
ASM_emit_operand_memory_size(x86MemoryScale size, bool destination)
{
	assert(ASM_is_scale(size) && "invalid x86Memory size.");

	if (destination)
	{
		switch (size)
		{
			case X86_MEM_SCALE_BYTE:
				printf("BYTE PTR ");
				break ;
			case X86_MEM_SCALE_WORD:
				printf("WORD PTR ");
				break ;
			case X86_MEM_SCALE_ARCH:					  
			case X86_MEM_SCALE_DWORD:
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
	ASM_emit_operand_memory_size(mem.size, destination);

	printf("[");
	ASM_emit_operand_register(mem.base);

	if (ASM_is_reg(mem.index))
	{
		printf(" + ");
		ASM_emit_operand_register(mem.index);

		if (ASM_is_scale(mem.scale))
		{
			printf(" * ");
			ASM_emit_operand_immediate(mem.scale);
		}
	}
	if (mem.displacement)
	{
		printf(" %c ", mem.displacement > 0 ? '+' : '-');
		ASM_emit_operand_immediate(mem.displacement);
	}
	printf("]");
}

void
ASM_emit_operand_symbol(x86Symbol sym)
{
	assert(sym != NULL && "invalid x86Symbol (null).");

	printf("[%s]", sym);
}

void
ASM_emit_symbol(x86Symbol sym)
{
	assert(sym != NULL && "invalid x86Symbol (null).");

	printf("%s", sym);
}

void
ASM_emit_operand(x86Operand op)
{
	assert(op.type < OPERAND_XENUM_LAST && "invalid x86Operand.");

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
			ASM_emit_operand_symbol(op.sym);
			break ;
		default:
			break ;
	}
}

void
ASM_emit_x86Instruction(x86Instruction instr, x86Operand *a, x86Operand *b)
{
	assert(instr < INSTRUCTION_XENUM_LAST && "invalid x86Instruction.");

	StringC	mnemonic = ASM_instruction_name(instr);

	printf("  %s%*.s", mnemonic, (int)(8 - strlen(mnemonic)), "");
	
	if (a)
		ASM_emit_operand(*a);
	if (b)
	{
		printf(", ");
		ASM_emit_operand(*b);
	}
	printf("\n");
}

