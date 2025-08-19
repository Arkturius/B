/**
 * types.h
 */

#ifndef _TYPES_H
# define _TYPES_H

# include <stdbool.h>
# include <stdint.h>

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

/**
 * Compiler specific types.
 */

# define	WORD_SIZE	4

typedef const char	*String;
typedef u32			StringIdx;
typedef u32			Size;
typedef i32         Offset;

typedef enum    _b_expr_type
{
    B_EXPR_IMMEDIATE,
    B_EXPR_REGISTER,
    B_EXPR_VARIABLE,
    B_EXPR_FUNCTION,
    B_EXPR_ROSTRING,
}   ExprType;

typedef struct  _b_expr
{
	ExprType    type;
    union
    {
        String  reg;
        String  name;
        u64     imm;
        Offset  off;
        void    *ptr;
    };
    bool        lval;
}	Expr;

#endif // _TYPES_H
