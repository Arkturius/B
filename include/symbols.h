/**
* symbols.h - Symbols handling.
*/

#if !defined (_B_SYMBOLS_H)
# define _B_SYMBOLS_H

# define XLIB_NO_PREFIX
# include <xlib.h>

x_enum
(
	StorageType,
	x_enum_prefix(STORAGE),
	x_enum_members 
	(
		(AUTO  ),
		(EXTERN),
		(INTERN),
	)
);

x_enum
(
	VarType,
	x_enum_prefix(VARIABLE),
	x_enum_members
	(
		(UNKNOWN ),
		(SCALAR  ),
		(VECTOR  ),
		(FUNCTION),
	)
);

typedef struct b_symbol
{
	StorageType	stype;
	VarType		vtype;
	StringC		name;
	Size		size;
	Offset		off;
}	Symbol;

x_array(Symbol, Symbols);

void
B_symbol_add(Symbol *symbol);

void
B_symbol_internal_add(Symbol *symbol);

inline bool
B_symbol_is_storage(Symbol *sym, StorageType storage)
{
	return (sym->stype == storage);
}

inline bool
B_symbol_is_variable(Symbol *sym, VarType variable)
{
	return (sym->vtype == variable);
}

Symbol
*B_symbol_find(StringC name);

void
B_symbol_dump(Symbol *symbol);

void
B_symbol_table_dump(void);

#endif // _B_SYMBOLS_H
