/**
 * symbols.h
 */

#if !defined (_SYMBOLS_H)
# define _SYMBOLS_H

# include <types.h>

typedef struct	_bsymbol		Symbol;
typedef enum	_bsymbol_type	SymbolType;

arr_decl(Symbol, Symbols);

enum _bsymbol_type
{
	SYMBOL_VARIABLE,
	SYMBOL_FUNCTION,
	SYMBOL_LABEL,
};

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
