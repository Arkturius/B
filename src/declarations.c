/**
 * declarations.c
 */

#include <b.h>
#include <codegen.h>

void
B_function_start(StringC name)
{
	B_symbol_new(SYMBOL_FUNCTION, name, 0);

	asm_label(name);
	asm_directive(DIRECTIVE_LONG,   .data = name, .off = WORD_SIZE);

	asm_push(REG(REG_EBP));
	asm_mov(REG(REG_EBP), REG(REG_ESP));
	asm_xor(REG(REG_EAX), REG(REG_EAX));
}

void
B_function_stop(StringC name)
{
	asm_mov(REG(REG_ESP), REG(REG_EBP));
	asm_pop(REG(REG_EBP));
	asm_ret();

	asm_directive(DIRECTIVE_GLOBAL, .data = name);
	printf("\n");
}

void
B_auto_decl(void)
{
	Scope	*current = arr_last(B.scopes);
	Size	to_sub = current->decl_size;

	asm_sub(REG(REG_ESP), IMM(to_sub));
	current->decl_size = 0;
}

void
B_auto_variable(StringC name, Size size)
{
	B_symbol_new(SYMBOL_VARIABLE, name, size);
}

void
B_extern_decl()
{

}

void
B_extern_variable(StringC name)
{

}
