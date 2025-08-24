/**
 * btypes.h
 */

#ifndef _BTYPES_H
# define _BTYPES_H

# include <stdbool.h>
# include <stdint.h>
# include <stdio.h>

/**
 * Integer types.
 */

typedef uint64_t	u64;
typedef uint32_t	u32;
typedef uint16_t	u16;
typedef uint8_t		u8;

typedef	uintptr_t	uptr;

typedef int64_t		i64;
typedef int32_t		i32;
typedef int16_t		i16;
typedef int8_t		i8;

typedef	intptr_t	iptr;

typedef float		f32;
typedef double		f64;

typedef u32			Size;
typedef i32         Offset;

# define    SHIFT_ARGS(ac, av)  (ac--, *av++)
# define    UNUSED(_x)          (void)(_x)

# define    B_log(_s, ...)                                              \
    dprintf(2, "[B]     log: "_s"\n", ##__VA_ARGS__);

# define    B_error(_s, ...)                                            \
    dprintf(2, "[B]   error: "_s"\n", ##__VA_ARGS__);

# define    B_warning(_s, ...)                                          \
    dprintf(2, "[B] warning: "_s"\n", ##__VA_ARGS__);

# define    _CONCAT(_a, _b) _a##_b
# define    CONCAT(_a, _b)  _CONCAT(_a, _b)
# define    STRINGIFY(_x)   #_x

# define VEC_IMPLEMENTATION
# include "vec.h"

# define	WORD_SIZE	4

typedef char        *String;

vec_decl(String);

typedef enum    b_expr_type
{
    EXPR_IMMEDIATE,
    EXPR_REGISTER,
    EXPR_VARIABLE,
    EXPR_FUNCTION,
    EXPR_ROSTRING,
}   ExprType;

typedef struct  b_expr
{
	ExprType    type;
    union
    {
        String  reg;
        String  name;
        Offset  off;
        i64     imm;
        void    *ptr;
    };
    bool        lval;
}	Expr;

typedef enum    b_symbol_type
{
    SYM_VARIABLE,
    SYM_PARAMETER,
    SYM_FUNCTION,
    SYM_EXTERNAL,
    SYM_LABEL,
}   SymbolType;

typedef struct  b_symbol
{
    SymbolType	type;
    Offset      off;
	String      name;
}	Symbol;

vec_decl(Symbol);

typedef struct  b_rostring
{
    String  name;
    String  text;
}   RoString;

vec_decl(RoString);

typedef struct  b_scope
{
    Offset  stack_offset;
    Size    sym_offset;
}   Scope;

vec_decl(Scope);

typedef struct  b_function
{
    String  name;
    Size    args_count;
    Size    save_caller;
    Size    save_callee;
    Scope  *scope;
}   Function;

typedef struct  b_loop
{
    Strings label_start;
    Strings label_end;
    Size    depth;
}   Loop;

/**
 * Generic SWAP
 */

# define    SWAP(_t, _a, _b) \
    do { _t _tmp = _a; _a = _b; _b = _tmp; } while (0)

#endif // _BTYPES_H
