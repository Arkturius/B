/**
 * B codegen tool.
 */

#include <bcodegen.h>

void	b_gen_section(SectionType t)
{
	String	sections[] =
	{
		[SECTION_TEXT]		= ".text",
		[SECTION_DATA]		= ".data",
		[SECTION_RODATA]	= ".rodata",
	};

	if (t >= SECTION_LAST)
		b_fatal("invalid section type", __func__);

	GEN(".section %s", sections[t]);
}

void	b_gen_program(void)
{
	GEN(".intel_syntax noprefix");
	b_gen_section(SECTION_TEXT);
}

void	b_gen_symbol_addr(String name)
{
	GEN(".long \"%s\" + %d", name, B_WORD_SIZE);
}

void	b_gen_function_start(String name)
{
	b_gen_section(SECTION_TEXT);
	GEN_LBL(name);
	b_gen_symbol_addr(name);
	
	GEN_OP2("enter", "0", "0");
	GEN_OP2("xor", EAX, EAX);
}

void	b_gen_function_end(void)
{
	GEN_OP("leave");
	GEN_OP("ret");
}
