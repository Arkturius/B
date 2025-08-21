/**
 * bcodegen.h
 */

#if !defined (_BCODEGEN_H)
# define _BCODEGEN_H

# include <btypes.h>
# include <bcontext.h>

typedef u32     VReg;

typedef void    (*Unary)(Vreg);

typedef struct  b_asmbackend
{
    
}   ASMBackend;

typedef struct  b_codebuilder
{
    ASMBackend  back;

}   CodeBuilder;

# if defined (B_NO_PREFIX)

# endif

extern  CodeBuilder  C;

#endif // _BCODEGEN_H
