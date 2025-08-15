/**
 * B compiler main header.
 */

#ifndef _BCOMPILER_H
# define _BCOMPILER_H

# include <stdarg.h>

# include <types.h>
# include <strtab.h>
# include <bloc.h>
# include <codegen.h>

void
B_program_start(void);

void
B_program_stop(void);

void	
B_rodata(void);


void
B_function(String name);

void
B_function_param(String name);

void
B_function_arg(Expr a);

Expr
B_function_call(Expr f);

void
B_if_block(Expr cond);

void
B_if_else_block(Expr cond);

void
B_if_end(void);

Expr
B_assign(u32 type, Expr lhs, Expr rhs);

Expr
B_logic_ternary(Expr cond, Expr yes, Expr no);

Expr
B_logic_or(Expr a, Expr b);

Expr
B_logic_and(Expr a, Expr b);

Expr
B_logic_xor(Expr a, Expr b);

Expr
B_comp_equal(Expr a, Expr b);

Expr
B_comp_not_equal(Expr a, Expr b);

Expr
B_comp_lower_than(Expr a, Expr b);

Expr
B_comp_lower_equal(Expr a, Expr b);

Expr
B_comp_greater_than(Expr a, Expr b);

Expr
B_comp_greater_equal(Expr a, Expr b);

Expr
B_op_shl(Expr x, Expr n);

Expr
B_op_shr(Expr x, Expr n);

Expr
B_op_add(Expr a, Expr b);

Expr
B_op_sub(Expr a, Expr b);

Expr
B_op_mul(Expr a, Expr b);

Expr
B_op_div(Expr a, Expr b);

Expr
B_op_mod(Expr a, Expr b);


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
B_expr_incr(Expr var);

Expr
B_expr_pre_incr(Expr var);

Expr
B_expr_pre_decr(Expr var);

Expr
B_expr_decr(Expr var);


Expr
B_builtin_char(Expr str, Expr idx);

Expr
B_builtin_lchar(Expr str, Expr idx, Expr chr);


typedef enum
{
	BCP_INIT	= 1 << 0,
}	CompilerFlags;

typedef enum b_symbol_type
{
	SYM_STACK_VAR,
}	SymbolType;

typedef struct b_symbol
{
	StringIdx	name;
	SymbolType	type;
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

	Symbols		symtab;
	RoStrings	rostrings;

	StringIdx	function;

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

# define	ONCE(X)		do { X } while (0)

# define	B_ERROR(X)		\
	ONCE(dprintf(2, "[ERROR] %s:%s:%d - %s\n", __func__, __FILE__, __LINE__, (X)); abort();)
# define	B_WARNING(X)	\
	ONCE(dprintf(2, "[WARN!] %s:%s:%d - %s\n", __func__, __FILE__, __LINE__, (X));)
# define	B_LOG(X, ...)	\
	ONCE(dprintf(2, "[DEBUG] "X"\n", ##__VA_ARGS__);)

bool
B_compiler_start(Compiler *cp)
{
	B_LOG("Compilation start.");

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

void
B_compiler_stop(Compiler *cp)
{
	if (!cp)
		return ;

	strtab_destroy(cp->symbols);
	strtab_destroy(cp->rodata);
	BLOC_DESTROY(bcp.symtab);
	
	B_LOG("Compilation stop.");
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
	B_LOG("%s", __func__);

	CG_directive_syntax();
	CG_directive_section(".text");
}

void
B_program_stop(void)
{
	B_LOG("%s", __func__);

	B_rodata();
}

void
B_rodata(void)
{
	B_LOG("%s", __func__);

	BLOC_FOREACH(RoString, rs, bcp.rostrings)
	{
		String	name = strtab_get(bcp.symbols, rs->name);
		String	text = strtab_get(bcp.symbols, rs->text);

		CG_label(name);
		CG_directive_long(WORD_SIZE, name);
		CG_directive_string(text);

		String	str_end = B_sprintf("%se", name);
		CG_label(str_end);
		CG_directive_set_len(name);
	}
}

void
B_function(String name)
{
	B_LOG("%s", __func__);

	// TODO: all memory stuff
	CG_directive_globl(name);
	CG_label(name);
	CG_directive_long(WORD_SIZE, name);
	CG_function_start(name);

	bcp.function = strtab_append(bcp.symbols, name);
	free((void *)name);
}

void
B_function_end()
{
	B_LOG("%s", __func__);

	String	name = strtab_get(bcp.symbols, bcp.function);
	String	end_label = B_sprintf(".%s.end", name);

	CG_label(end_label);
	CG_function_stop();
}

void
B_function_param(String name)
{
	StringIdx	idx = strtab_append(bcp.symbols, name);
	Symbol		new = (Symbol)
	{	
		.name = idx,
		.type = SYM_STACK_VAR
	};

	BLOC_APPEND(bcp.symtab, new);
	free((void *)name);
}

void
B_function_arg(Expr a) {}

Expr
B_function_call(Expr f) {}

void
B_return(void)
{
	JMP("");
	printf("simple return\n");
}

void
B_expr_return(Expr r)
{
	printf("returning expr\n");
}

void
B_if_block(Expr cond) {}

void
B_if_else_block(Expr cond) {}

void
B_if_end(void) {}

Expr
B_assign(u32 type, Expr lhs, Expr rhs) {}

Expr
B_logic_ternary(Expr cond, Expr yes, Expr no) {}

Expr
B_logic_or(Expr a, Expr b) {}

Expr
B_logic_xor(Expr a, Expr b) {}

Expr
B_logic_and(Expr a, Expr b) {}

Expr
B_comp_equal(Expr a, Expr b) {}

Expr
B_comp_not_equal(Expr a, Expr b) {}

Expr
B_comp_lower_than(Expr a, Expr b) {}

Expr
B_comp_lower_equal(Expr a, Expr b) {}

Expr
B_comp_greater_than(Expr a, Expr b) {}

Expr
B_comp_greater_equal(Expr a, Expr b) {}

Expr
B_op_shl(Expr x, Expr n) {}

Expr
B_op_shr(Expr x, Expr n) {}

Expr
B_op_add(Expr a, Expr b) {}

Expr
B_op_sub(Expr a, Expr b) {}

Expr
B_op_mul(Expr a, Expr b) {}

Expr
B_op_div(Expr a, Expr b) {}

Expr
B_op_mod(Expr a, Expr b) {}


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
B_expr_incr(Expr var) {}

Expr
B_expr_pre_incr(Expr var) {}

Expr
B_expr_pre_decr(Expr var) {}

Expr
B_expr_decr(Expr var) {}

Expr
B_builtin_char(Expr str, Expr idx) {}

Expr
B_builtin_lchar(Expr str, Expr idx, Expr chr) {}



#endif // BCOMPILER_IMPLEMENTATION

