/**
* symbols.h - Symbols handling.
*/

#if !defined (_B_SYMBOLS_H)
# define _B_SYMBOLS_H

# define XLIB_NO_PREFIX
# include <xlib.h>

x_enum 
(
	SymbolType,
	x_enum_prefix(SYMBOL),
	x_enum_members
	(
		(VAR_GLOBAL),
		(VAR_LOCAL ),
		(PARAMETER ),
		(FUNCTION  ),
		(LABEL     ),
		(TEMPORARY ),
	)
)

typedef struct b_symbol
{
	SymbolType	type;
	StringC		name;
	Size		size;
	union
	{
		Offset	var;
		Offset	param;
		StringC	label;
	};
}	Symbol;

x_array(Symbol, Symbols);

void
B_symbol_add(Symbol *symbol);

Symbol
*B_symbol_find(StringC name);

void
B_symbol_dump(Symbol *symbol);

#endif // _B_SYMBOLS_H
