/**
 * b.h
 */

#if !defined (_B_COMPILER_H)
# define _B_COMPILER_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <stdarg.h>

# include <types.h>

int
yyerror(const char *s);

# if defined (B_DEBUG)
#  define	BLOG(_s, ...)	printf("[B] > "_s"\n", ##__VA_ARGS__)
#  define	BTODO(_s, ...)	\
	do { \
		BLOG(_s, ##__VA_ARGS__); \
		abort(); \
	} while (0)
# else
#  define	BLOG(_s, ...)
#  define	BTODO(_s, ...)
# endif

# define	WORD_SIZE	4

typedef struct _bcompiler	Compiler;
typedef struct _bexpr		Expression;

typedef enum _bexpr_type
{
	EXPR_REGISTER,
}	ExpressionType;

struct _bexpr
{
	ExpressionType	type;
	union
	{
		u32		imm;
		Offset	var;
		String	reg;
	};
};

struct _bcompiler
{
	u32	flags;
};

extern Compiler	B;

bool
B_compiler_start(void);

bool
B_compiler_stop(void);

typedef enum _berror_type	ErrorType;

enum _berror_type
{
	ERROR_FILE,
	ERROR_ALLOC,
	ERROR_SYNTAX,
};

void
B_error_opt(ErrorType t, StringC fmt, ...);

# define	B_error(_t, ...)	B_error_opt(_t, ##__VA_ARGS__)


void
B_program_start(void);

void
B_program_stop(void);

void
B_function_start(String name);

void
B_function_stop(String name);

Expression
B_expression_primary(String identifier);

Expression
B_expression_constant(u64 value, String str, bool is_char);

#endif // _B_COMPILER_H
