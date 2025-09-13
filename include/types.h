/**
 * btypes.h
 */

#ifndef _TYPES_H
# define _TYPES_H

# include <stdbool.h>
# include <stdint.h>

# include <arr.h>

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

typedef char        *String;
typedef char const	*StringC;

arr_decl(String,	Strings);
arr_decl(StringC,	StringCs);

# define    shift_args(ac, av)  (ac--, *av++)
# define    unused(_x)          (void)(_x)

# if !defined (__noreturn)
#  define	__noreturn			__attribute__((noreturn))
# endif

typedef u32			Size;
typedef i32         Offset;

#endif // _TYPES_H
