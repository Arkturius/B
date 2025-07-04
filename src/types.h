/**
 * types.h
 */

#ifndef _TYPES_H
# define _TYPES_H

# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

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

# define	EAX "eax"
# define	EBX "ebx"
# define	ECX "ecx"
# define	EDX "edx"
# define	ESP "esp"
# define	EBP "ebp"
# define	EDI "edi"
# define	ESI	"esi"

# define	B_WORD_SIZE 4

# define	B_TABLE_CAP	1024

# define	B_TABLE_SIZE(_a)\
	(sizeof(_a)/sizeof((_a)[0])

# define	B_TABLE_DECL(_a, _t)\
	typedef struct _a##_s { u32 count; _t table[B_TABLE_CAP]; } _a

# define	B_TABLE_PUSH(_a, _e)\
	if ((_a).count < B_TABLE_CAP) { (_a).table[(_a).count++] = (_e); }

# define	B_TABLE_NEXT(_a)\
	&((_a).table[(_a).count])

typedef enum b_sections
{
	SECTION_TEXT,
	SECTION_DATA,
	SECTION_RODATA,

	SECTION_LAST,
}	SectionType;

typedef enum b_constant_type
{
	CONSTANT_INT,
	CONSTANT_STR,
	CONSTANT_CHR,

	CONSTANT_LAST,
}	ConstantType;

typedef enum b_symbol_type
{
	SYMBOL_AUTOVAR,
	SYMBOL_FUNCTION,

	SYMBOL_LAST,
}	SymbolType;

typedef u32		Size;
typedef u32		WSize;
typedef u32		Offset;
typedef char	*String;

typedef struct
{
	WSize	size;
	Offset	off;
}	Scope;

typedef struct
{
	String		name;
	SymbolType	type;
	WSize		size;
	Offset		off;
}	Symbol;

typedef Symbol	Function;
typedef String	Register;
typedef String	Label;

B_TABLE_DECL(Functions,	Function);
B_TABLE_DECL(Symbols,	Symbol);
B_TABLE_DECL(Scopes,	Scope);
B_TABLE_DECL(Registers,	Register);
B_TABLE_DECL(Labels,	Label);

typedef struct compiler_s
{
	Functions	functions;
	Symbols		symbols;
	Scopes		scopes;
	Registers	registers;
	Labels		labels;
}	BCompiler;

typedef enum e_exprtype
{
	EXPR_CONSTANT,
	EXPR_STRING,
	EXPR_AUTOVAR,
}	ExprType;

typedef struct expression_s
{
	ExprType	type;
	union
	{
		u64			val;
		i64			ival;
		String		str;
		Register	reg;
	};
}	Expression;

static inline void	b_fatal(const char *msg, const char *fn)
{
	printf("fatal: %s: %s\n", fn, msg);
	exit(1);
}

#endif // _TYPES_H
