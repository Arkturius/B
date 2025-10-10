/**
* B - a syntax-driven B compiler.
*/

#if !defined (_B_COMPILER_H)
# define _B_COMPILER_H

# include <assert.h>

# define XLIB_NO_PREFIX
# include <xlib.h>

# include <symbols.h>
# include <scopes.h>

# include <eval/control.h>
# include <eval/expression.h>
# include <eval/computing.h>
# include <eval/declaration.h>

# include <codegen/codegen.h>
# include <codegen/regalloc.h>

extern int			yyleng;
extern int			yylineno;
extern char			*yytext;

extern int			b_col;
extern const char	*b_row_start;
extern char			b_hold_char;

typedef struct b_compiler
{
	Scopes		scopes;
	Symbols		symbols;
	Symbols		internals;

	LabelFrame	labels;

	ROStrings	rostrings;

	Size		auto_size;
	Size		param_size;

	ArgumentStack	arguments;

	StringC		function_name;

	Expressions	ivals;

	SectionType	section;
	
	String		input_file;
	String		directory;
}	Compiler;

extern Compiler	B;

bool
B_compiler_start(void);

void
B_compiler_stop(void);

_noreturn void
B_compiler_error(StringC reason);

StringC
B_asprintf(StringC fmt, ...);

# include <bdebug.h>

#endif // _B_COMPILER_H
