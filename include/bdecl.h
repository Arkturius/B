/**
 * bdecl.h
 */

#if !defined(_BDECL_H)
# define _BDECL_H

# include <btypes.h>
# include <bcontext.h>

# if defined(B_NO_PREFIX)
#  define   function_start  B_function_start
#  define   function_stop   B_function_stop
# endif

void
B_function_start(String name);

void
B_function_stop(void);

#endif // _BDECL_H
