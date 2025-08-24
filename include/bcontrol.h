/**
 * bcontrol.h
 */

#if !defined (_BCONTROL_H)
# define _BCONTROL_H

# include <btypes.h>

# if defined (B_NO_PREFIX)
#  define   return_expr     B_return_expr
# endif

void
B_return_expr(Expr ret);

#endif // _BCONTROL_H
