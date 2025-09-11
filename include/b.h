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
# include <symbols.h>
# include <expression.h>

int
yyerror(const char *s);

#  define	BLOG(_s, ...)	printf("[B] > "_s"\n", ##__VA_ARGS__)
#  define	BTODO(_s, ...)					\
	do {									\
		BLOG("TODO: "_s, ##__VA_ARGS__);	\
		abort();							\
	} while (0)

# define	WORD_SIZE	4

typedef struct _bcompiler	Compiler;
typedef struct _bexpression	Expression;
typedef struct _bscope		Scope;

arr_decl(Scope,  Scopes);

struct _bscope
{
	Size	sym_start;
	Size	sym_count;
	Size	decl_size;
	Size	stack;
};

struct _bloop
{
	
};

struct _bcompiler
{
	u32	flags;

	Symbols	symbols;
	Scopes	scopes;
	
	StringC	function;
};

extern Compiler	B;

StringC
B_asprintf(StringC fmt, ...);

void
B_arena_erase(Size size);

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
	ERROR_SYMBOL,

	ERROR_ENUM_MAX,
};

void
B_error_opt(ErrorType t, StringC fmt, ...);

# define	B_error(_t, ...)	B_error_opt(_t, ##__VA_ARGS__)


void
B_program_start(void);

void
B_program_stop(void);

void
B_scope_start(void);

void
B_scope_stop(void);

void
B_symbol_new(SymbolType type, StringC name, Size size);


void
B_function_start(StringC name);

void
B_function_stop(StringC name);



Expression
B_expression_variable(StringC name);

Expression
B_expression_constant(u64 value, StringC str, bool is_char);


Expression
B_expression_assignment(u32 type, Expression lhs, Expression rhs);

#endif // _B_COMPILER_H
