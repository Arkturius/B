/**
* codegen.c
*/

# include <eval/expression.h>

# include <codegen/regalloc.h>
# include <codegen/emission.h>

void
CG_program_header(void)
{
	ASM_dir_syntax();
	ASM_dir_section(SECTION_TEXT);
	printf("\n");
}

void
CG_rodata()
{
	todo("dump all rostrings to form .rodata section");
}

void
CG_label(StringC label, bool user)
{
	ASM_label(label);
	if (user)
		ASM_dir_long(label, WORD_SIZE);
}

void
CG_prolog(void)
{
	x86Operand	ebp = REG_OPERAND(X86_REG_EBP);
	x86Operand	esp = REG_OPERAND(X86_REG_ESP);

	ASM_push(ebp);
	ASM_mov(ebp, esp);
}

void
CG_epilog(void)
{
	x86Operand	ebp = REG_OPERAND(X86_REG_EBP);

	ASM_pop(ebp);
	ASM_ret();
}

void
CG_return(Expression e)
{


//	ASM_mov(eax, ret);
}
