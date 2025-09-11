/**
 * program.c
 */

#include "arr.h"
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
	arr_foreach(Symbol, sym, B.symbols)
	{
		printf("Symbol - { ");
		printf("type = %d, ", sym->type);
		printf("name = %s, ", sym->name);
		printf("size = %d, ", sym->size);
		printf("off = %d", sym->off);
		printf("}\n");
	}

	B_rodata();
}

void
B_scope_start(void)
{
	Scope	*current = arr_last(B.scopes);

	Scope	new = 
	{
		.decl_size	= 0,
		.sym_count	= 0,
		.sym_start	= arr_count(B.symbols),
		.stack		= current ? current->stack : 0,
	};
	arr_append(B.scopes, new);
}

void
B_scope_stop(void)
{
	Scope	*current = arr_last(B.scopes);

	if (arr_count(B.scopes) == 0)
		B_error(ERROR_SYNTAX, "trying to exit global scope.");

	arr_pop(B.symbols, current->sym_count);
	arr_pop(B.scopes, 1);
}
