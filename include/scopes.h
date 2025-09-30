/**
* symbols.h - Symbols handling.
*/

#if !defined (_B_SCOPES_H)
# define _B_SCOPES_H

# define XLIB_NO_PREFIX
# include <xlib.h>

# include <symbols.h>

typedef struct b_scope
{
	Size	start;
	Size	count;
}	Scope;

x_array(Scope, Scopes);

void
B_scope_enter(void);

void
B_scope_leave(void);

void
B_scope_grow(Symbol *symbol);

#endif // _B_SCOPES_H
