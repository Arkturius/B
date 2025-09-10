/**
 * program.c
 */

#include <b.h>
#include <codegen.h>

static void
B_rodata()
{

}

void
B_program_start(void)
{
	asm_directive(DIRECTIVE_ALIGN,	 .value = 16);
	asm_directive(DIRECTIVE_SYNTAX,	 .data = SYNTAX(SYNTAX_INTEL));
	asm_directive(DIRECTIVE_SECTION, .data = SECTION(SECTION_TEXT));
	printf("\n");
}

void
B_program_stop(void)
{
	B_rodata();
}

void
B_function_start(String name)
{
	asm_label(name);
	asm_directive(DIRECTIVE_LONG,   .data = name, .off = WORD_SIZE);

	asm_push(REG_EBP);
	asm_mov(REG_EBP, REG_ESP);
	asm_xor(REG_EAX, REG_EAX);
}

void
B_function_stop(String name)
{
	asm_mov(REG_ESP, REG_EBP);
	asm_pop(REG_EBP);
	asm_ret();

	asm_directive(DIRECTIVE_GLOBAL, .data = name);
	printf("\n");
}
