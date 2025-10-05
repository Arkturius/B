/**
* scopes.c
*/

#include <b.h>

void
B_scope_enter(void)
{
	Scope	new = 
	{
		.start = arr_count(B.symbols),
		.count = 0,
	};
	arr_append(B.scopes, new);

//	warning("\033[31m %s: scopes.count = %d \033[0m", __func__, arr_count(B.scopes));
}

void
B_scope_leave(void)
{
	if (arr_count(B.scopes) <= 1)
		B_compiler_error("Compiler can't leave global scope.");

	B_symbol_table_dump();

	Scope	*current     = arr_last(B.scopes);

	arr_pop(B.scopes, 1);
	arr_pop(B.symbols, current->count);

//	warning("\033[31m %s: scopes.count = %d \033[0m", __func__, arr_count(B.scopes));
}

void
B_scope_grow(Symbol *symbol)
{
	if (arr_count(B.scopes) < 1)
		B_compiler_error("No global scope found. aborting.");

	Scope	*current = arr_last(B.scopes);

	current->count++;
}
