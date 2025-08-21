/**
 * bsymbol.h
 */

#if !defined(_BSYMBOL_H)
# define _BSYMBOL_H

# include <btypes.h>
# include <bcontext.h>

# if defined(B_NO_PREFIX)
#  define   symbol_new      B_symbol_new
#  define   symbol_del      B_symbol_del
#  define   symbol_lookup   B_symbol_lookup
#  define   symbol_dump     B_symbol_dump
#  define   scope_enter     B_scope_enter
#  define   scope_exit      B_scope_exit
# endif

void
B_symbol_new(String name, SymbolType type);

void
B_symbol_del(String name);

Symbol
*B_symbol_lookup(String name);

void
B_symbol_dump(void);

void
B_scope_enter(void);

void
B_scope_exit(void);

#endif // _BSYMBOL_H
