/**
 * symbols.c
 */

#include "symbols.h"
#include <b.h>
#include <string.h>

static Offset
B_symbol_offset(Size sym_size)
{
	Scope	*current;

	if (B.scopes.count == 0)
		B_error(ERROR_SYMBOL, "no current scope for symbol.");

	current = arr_last(B.scopes);
	return (-(current->stack + WORD_SIZE));
}

static void
B_symbol_register(Symbol sym)
{
	Scope	*current;

	if (B.scopes.count == 0)
		B_error(ERROR_SYMBOL, "no current scope for symbol.");

	current = arr_last(B.scopes);

	if (sym.size & (WORD_SIZE - 1))
		sym.size = (sym.size + WORD_SIZE) % WORD_SIZE;

	current->stack		+= sym.size;
	current->decl_size	+= sym.size;
	current->sym_count	+= 1;
}

Symbol
*B_symbol_find(StringC name)
{
	arr_foreach_rev(Scope, scope, B.scopes)
	{
		Symbols	scope_symbols = 
		{
			.count = scope->sym_count,
			.items = arr_first(B.symbols) + scope->sym_start
		};
		arr_foreach(Symbol, symbol, scope_symbols)
		{
			if (strcmp(symbol->name, name) == 0)
				return (symbol);
		}
	}
	return (NULL);
}

void
B_symbol_new(SymbolType type, StringC name, Size size)
{
	Symbol	sym;

	switch (type)
	{
		case SYMBOL_FUNCTION:
		case SYMBOL_LABEL:
		{
			sym = (Symbol)
			{
				.type	= type,
				.name	= name,
				.off	= 0,
				.size	= 0,
			};
			break ;
		}
		case SYMBOL_VARIABLE:
		{
			Offset	off = B_symbol_offset(size);

			sym = (Symbol)
			{
				.type	= SYMBOL_VARIABLE,
				.name	= name,
				.off	= off,
				.size	= size,
			};
			B_symbol_register(sym);
			break ;
		}
		default:
			B_error(ERROR_SYMBOL, "unknown symbol type.");
	}

	arr_append(B.symbols, sym);
}
