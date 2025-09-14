/**
 * symbols.c
 */

#include "arr.h"
#include <b.h>
#include <string.h>

static inline Offset
B_auto_offset(Size sym_size)
{
	Scope	*current;

	if (B.scopes.count == 0)
		B_error(ERROR_SYMBOL, "no current scope for symbol.");

	current = arr_last(B.scopes);
	return (-(current->stack + WORD_SIZE));
}

static inline Offset
B_param_offset(void)
{
	return (2 + B.function.arg_count) * WORD_SIZE;
}

Symbol
*B_symbol_find(StringC name)
{
	Size	start = 0;

	if (arr_count(B.scopes) == 0)
		B_error(ERROR_SYMBOL, "no scope for symbol research.");

	arr_foreach_rev(Scope, scope, B.scopes)
	{
		start = scope->sym_start;
		Symbols	scope_symbols = 
		{
			.count = scope->sym_count,
			.items = arr_first(B.symbols) + start,
		};
		arr_foreach(Symbol, symbol, scope_symbols)
		{
			if (strcmp(symbol->name, name) == 0)
				return (symbol);
		}
	}
	Symbols	params = 
	{
		.count = B.function.arg_count,
		.items = arr_first(B.symbols) + start - B.function.arg_count,
	};
	arr_foreach(Symbol, param, params)
	{
		if (strcmp(param->name, name) == 0)
			return (param);
	}

	Symbols	functions = 
	{
		.count = start,
		.items = arr_first(B.symbols),
	};
	arr_foreach(Symbol, function, functions)
	{
		if (strcmp(function->name, name) == 0)
			return (function);
	}
	return (NULL);
}

void
B_symbol_new(SymbolType type, StringC name, Size size)
{
	Symbol	sym;

	sym.type = type;
	sym.name = name;
	sym.size = size;
	switch (type)
	{
		case SYMBOL_FUNCTION:
		case SYMBOL_LABEL:
		{
			sym.off = 0;
			break ;
		}
		case SYMBOL_VARIABLE:
		{
			sym.off = B_auto_offset(size);
			break ;
		}
		case SYMBOL_PARAMETER:
		{
			sym.type = SYMBOL_VARIABLE;
			sym.off = B_param_offset();
			break ;
		}
		default:
			B_error(ERROR_SYMBOL, "unknown symbol type.");
	}
	arr_append(B.symbols, sym);
}
