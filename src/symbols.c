/**
* symbols.c
*/

#include <b.h>
#include <string.h>

void
B_symbol_add(Symbol *symbol)
{
	B_DBG_TREE;

	B_scope_grow(symbol);
	arr_append(B.symbols, *symbol);
}

void
B_symbol_internal_add(Symbol *symbol)
{
	B_DBG_TREE;

	arr_append(B.internals, *symbol);
}

Symbol
*B_symbol_find(StringC name)
{
	B_DBG_TREE;

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
	free((String)internal_name); // todo arena management
	
	scope_symbols = x_subarray(B.symbols, global_scope->start, global_scope->count);
	arr_foreach(Symbol, symbol, scope_symbols)
	{
		if (!strcmp(symbol->name, name))
			return (symbol);
	}

	return (NULL);
}

bool
B_is_symbol(Symbol *sym)
{
	if (!sym)
		return (false);

	arr_foreach(Symbol, symbol, B.symbols)
	{
		if (symbol == sym)
			return (true);
	}
	return (false);
}

void
B_symbol_dump(Symbol *symbol)
{
	dprintf 
	(
		2,
		"Symbol "
		"{ "
		" \033[38;1mname\033[0m = %p:%16s,"
		" \033[38;1mstorage type\033[0m = %16s,"
		" \033[38;1mvariable type\033[0m = %18s,"
		" \033[38;1moffset\033[0m = %4d,"
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

	return ;

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
