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

void
B_symbol_internal_add(Symbol *symbol)
{
	arr_append(B.internals, *symbol);
}

Symbol
*B_symbol_find(StringC name)
{
	Scope	*global_scope = NULL;
	Symbols	scope_symbols;

	if (!arr_count(B.scopes))
		B_compiler_error("no global scope.");

	arr_foreach_rev(Scope, scope, B.scopes)
	{
		if (scope == arr_first(B.scopes))
		{
			global_scope = scope;
			break ;
		}
		scope_symbols = x_subarray(B.symbols, scope->start, scope->count);
		arr_foreach(Symbol, symbol, scope_symbols)
		{
			if (!strcmp(symbol->name, name))
				return (symbol);
		}
	}

	StringC	internal_name = B_asprintf("%s.%s", B.function_name, name);
	arr_foreach(Symbol, internal, B.internals)
	{
		if (!strcmp(internal->name, internal_name))
		{
			free((String)internal_name);
			return (internal);
		}
	}
	free((String)internal_name);
	
	scope_symbols = x_subarray(B.symbols, global_scope->start, global_scope->count);
	arr_foreach(Symbol, symbol, scope_symbols)
	{
		if (!strcmp(symbol->name, name))
			return (symbol);
	}

	return (NULL);
}

void
B_symbol_dump(Symbol *symbol)
{
	dprintf 
	(
		2,
		"Symbol "
		"{ "
		" name = %p:%16s,"
		" storage type = %16s,"
		" variable type = %16s,"
		" offset = %4d,"
		"}",
		symbol->name, symbol->name,
		x_tostr_StorageType(symbol->stype),
		x_tostr_VarType(symbol->vtype),
		symbol->off
	);
}

void
B_symbol_table_dump(void)
{
	Symbols	scope_symbols;

	dprintf(2, "-----------------------------------------------------\n");
	arr_foreach(Scope, scope, B.scopes)
	{
		scope_symbols = x_subarray(B.symbols, scope->start, scope->count);
		arr_foreach(Symbol, symbol, scope_symbols)
		{
			dprintf(2, "[%lu] - ", scope - arr_first(B.scopes));
			B_symbol_dump(symbol);
			dprintf(2, "\n");
		}
	}
	arr_foreach(Symbol, internal, B.internals)
	{
		dprintf(2, "[I] - ");
		B_symbol_dump(internal);
		dprintf(2, "\n");
	}
	dprintf(2, "-----------------------------------------------------\n");
}
