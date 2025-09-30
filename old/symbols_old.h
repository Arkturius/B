/**
 * symbols.h
 */

#if !defined (_SYMBOLS_H)
# define _SYMBOLS_H

# define XLIB_NO_PREFIX
# include <xlib.h>

typedef struct	_bsymbol		Symbol;

x_array(Symbol, Symbols);

x_enum 
(
	SymbolType,
	x_enum_prefix(SYMBOL),
	x_enum_members
	(
		(VARIABLE ),
		(FUNCTION ),
		(PARAMETER),
		(LABEL    ),
		(EXTERN   ),
	)
)

struct _bsymbol
{
	SymbolType	type;
	StringC		name;
	Size		size;
	Offset		off;
};

Symbol
*B_symbol_find(StringC name);

void
B_symbol_new(SymbolType type, StringC name, Size size);

void
B_auto_decl(void);

void
B_auto_variable(StringC name, Size size);

void
B_extern_decl(void);

void
B_extern_variable(StringC name);

#endif // _SYMBOLS_H
