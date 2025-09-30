/**
* symbols.c
*/

#include <b.h>
#include <string.h>

void
B_symbol_add(Symbol *symbol)
{
	B_scope_grow(symbol);
	arr_append(B.symbols, *symbol);
}

Symbol
*B_symbol_find(StringC name)
{
	arr_foreach_rev(Scope, current, B.scopes)
	{
		if (current == arr_first(B.scopes))
			break ;

		Symbols	symbols = x_subarray(B.symbols, current->start, current->count);

		arr_foreach(Symbol, symbol, symbols)
		{
			if (strcmp(symbol->name, name) == 0)
				return (symbol);
		}
	}

	// TODO: there will be SIMPLE DEFINITIONS, EXTERNAL VECTORS here.
	Symbols	functions = x_subarray(B.symbols, 0, arr_first(B.scopes)->count);

	arr_foreach(Symbol, function, functions)
	{
		if (strcmp(function->name, name) == 0)
			return (function);
	}

	unreachable("no symbol found.");
}

void
B_symbol_dump(Symbol *symbol)
{
	printf
	(
		"Symbol\n"
		"{\n"
		"    type = %s\n"
		"}\n",
		x_tostr_SymbolType(symbol->type)
	);
}
