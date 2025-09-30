/**
 * codegen.h
 */

#if !defined (_B_CODEGEN_H)
# define _B_CODEGEN_H

# define XLIB_NO_PREFIX
# include <xlib.h>

void
CG_program_header(void);

void
CG_rodata(void);

void
CG_label(StringC label, bool user);

void
CG_prolog(void);

void
CG_epilog(void);

#endif // _B_CODEGEN_H
