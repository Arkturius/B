/**
 * B compiler main header.
 */

#ifndef _BCOMPILER_H
# define _BCOMPILER_H

#include <stdarg.h>
# include <types.h>
# include <strtab.h>
# include <bloc.h>
# include <codegen.h>

# define	WORD_SIZE	4

void
B_program_start(void);

void
B_program_stop(void);

void	
B_rodata(void);


Expr
B_expr_constant(i64 number);

Expr
B_expr_string(String str);

Expr
B_expr_char(String str);

Expr
B_expr_variable(String var);

Expr
B_expr_lvalue(String var);

Expr
B_expr_deref(Expr ptr);

Expr
B_expr_subscript(Expr arr, Expr idx);

Expr
B_expr_addrof(Expr var);

Expr
B_expr_negate(Expr var);

Expr
B_expr_invert(Expr var);


Expr
B_builtin_char(Expr str, Expr idx);

Expr
B_builtin_lchar(Expr str, Expr idx, Expr chr);

typedef enum
{
	BCP_INIT	= 1 << 0,
}	CompilerFlags;

typedef struct b_symbol
{
	StringIdx	name;
	uint32_t	type;
}	Symbol;

BLOC_DECL(Symbol);

typedef struct b_rostring
{
	StringIdx	text;
	StringIdx	name;
}	RoString;

BLOC_DECL(RoString);

typedef struct b_compiler
{
	u32			flags;

	RoStrings	rostrings;

	Strtab		symbols;
	Strtab		rodata;
}	Compiler;

extern Compiler	bcp;

#endif // _BCOMPILER

#define BCOMPILER_IMPLEMENTATION
#if defined(BCOMPILER_IMPLEMENTATION)

# include <stdio.h>

# define STRTAB_IMPLEMENTATION
# include <strtab.h>

# define BLOC_IMPLEMENTATION
# include <bloc.h>

Compiler	bcp = {0};
char		btmp[1024] = {0};

# define	DESTRUCTOR	__attribute__((destructor))
# define	ONCE(X)		do { X } while (0)

# define	B_ERROR(X)		\
	ONCE(printf("[ERROR] %s:%s:%d - %s\n", __func__, __FILE__, __LINE__, (X)); abort();)
# define	B_WARNING(X)	\
	ONCE(printf("[WARN!] %s:%s:%d - %s\n", __func__, __FILE__, __LINE__, (X));)
# define	B_LOG(X, ...)	\
	ONCE(printf("[DEBUG] "X"\n", ##__VA_ARGS__);)

bool
B_compiler_start(Compiler *cp)
{
	cp->flags = BCP_INIT;

	cp->symbols = strtab_init(4096);
	if (!cp->symbols)
	{
		B_WARNING("symbols strtab allocation");
		return false;
	}
	cp->rodata = strtab_init(4096);
	if (!cp->rodata)
	{
		B_WARNING("rodata strtab allocation");
		return false;
	}

	return true;
}

DESTRUCTOR void
B_compiler_stop(Compiler *cp)
{
	if (!cp)
		return ;

	strtab_destroy(cp->symbols);
	strtab_destroy(cp->rodata);
}

String
B_sprintf(const char *fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	vsprintf(btmp, fmt, ap);
	va_end(ap);

	return btmp;
}


void
B_program_start(void)
{
	CG_directive_syntax();
	CG_directive_section(".text");
}

void
B_program_stop(void)
{
	B_rodata();
}

void
B_rodata(void)
{
	char	buffer[32] = {0};

	BLOC_FOREACH(RoString, rs, bcp.rostrings)
	{
		String	name = strtab_get(bcp.symbols, rs->name);
		String	text = strtab_get(bcp.symbols, rs->text);

		CG_label(name);
		CG_directive_long(WORD_SIZE, name);
		CG_directive_string(text);

		sprintf(buffer, "%se", name);
		CG_label(buffer);

		sprintf(buffer, "%ss", name);
		CG_directive_set(buffer, B_sprintf(".%se - .%s", name, name));
	}
}








Expr
B_expr_constant(i64 number) {}

Expr
B_expr_string(String str) {}

Expr
B_expr_char(String str) {}

Expr
B_expr_variable(String var) {}

Expr
B_expr_lvalue(String var) {}

Expr
B_expr_deref(Expr ptr) {}

Expr
B_expr_subscript(Expr arr, Expr idx) {}

Expr
B_expr_addrof(Expr var) {}

Expr
B_expr_negate(Expr var) {}

Expr
B_expr_invert(Expr var) {}


Expr
B_builtin_char(Expr str, Expr idx) {}

Expr
B_builtin_lchar(Expr str, Expr idx, Expr chr) {}

#endif // BCOMPILER_IMPLEMENTATION

