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

#  define	BLOG(_s, ...)	dprintf(2, "[B] > "_s"\n", ##__VA_ARGS__)
#  define	BTODO(_s, ...)					\
	do {									\
		BLOG("TODO: "_s, ##__VA_ARGS__);	\
		abort();							\
	} while (0)

# define	WORD_SIZE	4

arr_decl(Offset, Offsets);

typedef struct _bcompiler	Compiler;
typedef struct _bscope		Scope;
typedef struct _bfunction	Function;
typedef struct _brostring	RoString;

arr_decl(Scope,  Scopes);

struct _bscope
{
	Size	sym_start;
	Size	sym_count;
	Size	decl_size;
	Size	stack;
};

typedef enum _blabel_type
{
	LABEL_NULL,
	LABEL_FUNC_STOP,
	LABEL_LOOP_START,
	LABEL_LOOP_STOP,
	LABEL_SKIP_IF,
	LABEL_SKIP_ELSE,

	LABEL_ENUM_MAX,
}	LabelType;

typedef struct _blabel	Label;
typedef u32				LabelID;

struct _blabel
{
	LabelID	id;
	StringC	name;
};

arr_decl(Label,	Labels);

typedef struct _blabel_grid
{
	Labels	grid[LABEL_ENUM_MAX];
	Size	next_id;
}	LabelGrid;

void
B_label_push(LabelType type);

void
B_label_pop(LabelType type);

Label
B_label_get(LabelType type);

struct _bfunction
{
	StringC	name;
	Size	arg_count;
	Size	callee_save;
	Size	caller_save;
};

struct _brostring
{
	StringC	text;
	StringC	name;
};

arr_decl(RoString, RoStrings);

struct _bcompiler
{
	u32	flags;

	Symbols		symbols;
	RoStrings	rostrings;
	
	Scopes		scopes;
	
	RegFrame	frame;
	
	Function	function;

	Expressions	arguments;
	Offsets		arities;
	
	LabelGrid	labels;
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
	ERROR_SYMBOL,
	ERROR_ASM,

	ERROR_ENUM_MAX,
};

__noreturn void
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
B_return_expr(Expression ret);

void
B_if_start(Expression cond);

void
B_if_stop(bool is_else);

void
B_else_stop(void);

void
B_while_start(void);

void
B_while_condition(Expression cond);

void
B_while_stop(void);

void
B_function_start(StringC name);

void
B_function_stop(StringC name);

void
B_function_param(StringC name);

Expression
B_function_call(Expression call);

void
B_function_argument(Expression arg);

#endif // _B_COMPILER_H
