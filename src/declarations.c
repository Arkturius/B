/**
 * declarations.c
 */

#include <b.h>
#include <codegen.h>

void
B_function_start(StringC name)
{
	B_symbol_new(SYMBOL_FUNCTION, name, 0);
	B_label_push(LABEL_FUNC_STOP);

	emit_label(name);
 	emit_directive(DIRECTIVE_LONG, .data = name, .off = WORD_SIZE);

	asm_push(EBP);
	code_move(EBP, ESP);
}

void
B_function_stop(StringC name)
{
	code_label(LABEL_FUNC_STOP);
	code_move(ESP, EBP);
	asm_pop(EBP);
	asm_ret();

 	emit_directive(DIRECTIVE_GLOBAL, .data = name);

	arr_pop(B.symbols, B.function.arg_count);
	B.function = (Function){0};

	B_label_pop(LABEL_FUNC_STOP);
}

void
B_function_param(StringC name)
{
	B_symbol_new(SYMBOL_PARAMETER, name, WORD_SIZE);
	B.function.arg_count++;
}

static void
B_auto_register(Symbol sym)
{
	Scope	*current;

	if (arr_count(B.scopes) == 0)
		B_error(ERROR_SYMBOL, "no current scope for symbol.");

	current = arr_last(B.scopes);

	current->stack		+= sym.size;
	current->decl_size	+= sym.size;
	current->sym_count	+= 1;
}

void
B_auto_decl(void)
{
	Scope	*current = arr_last(B.scopes);
	Size	to_sub = current->decl_size;

	current->decl_size = 0;

	asm_sub(ESP, IMM(to_sub));
}

void
B_auto_variable(StringC name, Size size)
{
	B_symbol_new(SYMBOL_VARIABLE, name, size);
	B_auto_register(*arr_last(B.symbols));
}

void
B_extern_decl()
{
	
}

void
B_extern_variable(StringC name)
{

}
