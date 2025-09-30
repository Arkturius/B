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
}

void
B_scope_leave(void)
{
	if (arr_count(B.scopes) <= 1)
		B_compiler_error("Compiler can't leave global scope.");

	Scope	*current = arr_last(B.scopes);

#ifdef B_DUMP_ALL

	Symbols	symbols = x_subarray(B.symbols, arr_count(B.symbols), current->count);

	arr_foreach(Symbol, symbol, symbols)
	{
		B_symbol_dump(symbol);
	}

#endif

	arr_pop(B.symbols, current->count);
	arr_pop(B.scopes, 1);
}

void
B_scope_grow(Symbol *symbol)
{
	if (arr_count(B.scopes) < 1)
		B_compiler_error("No global scope found. aborting.");

	Scope	*current = arr_last(B.scopes);

	current->count++;
}
