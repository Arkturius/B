/**
 * declarations.c
 */

#include "arr.h"
#include "expression.h"
#include <b.h>
#include <codegen.h>

void
B_function_start(StringC name)
{
	B_symbol_new(SYMBOL_FUNCTION, name, 0);
	B_label_push(LABEL_FUNC_STOP);

	emit_directive(DIRECTIVE_ALIGN,	  .value = 16);
	emit_label(name);
 	emit_directive(DIRECTIVE_LONG, .data = name, .off = WORD_SIZE);

	asm_push(EBP);
	code_move(EBP, ESP);
	asm_sub(ESP, IMM(3 * WORD_SIZE));
// 	asm_push(EDI);
// 	asm_push(EDI);
// 	asm_push(EDI);
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
	printf("\n");
}

void
B_function_param(StringC name)
{
	B_symbol_new(SYMBOL_PARAMETER, name, WORD_SIZE);
	B.function.arg_count++;
}

Expression
B_function_call(Expression call)
{
	if (call.type == EXPR_IMMEDIATE)
		B_error(ERROR_ASM, "can't call on immediate values.'");

	Size	arity    = *arr_last(B.arities);
	Offset	arg_off  = arity * WORD_SIZE;
	Offset	off_save = arg_off;

	Expressions	call_args =
	{
		.count = arity,
		.capacity = arity,
		.items = arr_last(B.arguments) - arity + 1,
	};
	
	int	spill_eax = 0;

	arr_foreach_rev(Expression, arg, call_args)
		spill_eax += (arg->type == EXPR_REGISTER && arg->reg == REG_EAX);

	if (!spill_eax && B.frame.states[REG_EAX].in_use)
		register_spill(REG_EAX);

	asm_sub(ESP, IMM(arg_off)); // TODO : use last arg slots if needed;
								//
	arr_foreach_rev(Expression, arg, call_args)
	{
		BLOG("current arg = %p", arg);
		arg_off -= WORD_SIZE;
		Expression	arg_slot = 
		{
			.type = EXPR_MEMORY,
			.mem = 
			{
				.base = REG_ESP,
				.displacement = arg_off,
			},
		};
		code_move(arg_slot, *arg);
	}
	arr_pop(B.arguments, arity);
	arr_pop(B.arities, 1);

//	asm("int3");

	code_call(call);
	
	Expression	ret = EAX;
	
	B.frame.states[REG_EAX].in_use = true;
	if (B.frame.states[REG_EAX].spilled)
	{
		ret = REG(register_alloc(REG_NULL));
		code_move(ret, EAX);
	}

	asm_add(ESP, IMM(off_save)); // TODO: make the immediate offset match what was added before call
	
	register_restore(REG_EAX);
	
	return (ret);
}

void
B_function_invoke(void)
{
	arr_append(B.arities, 0);
}

void
B_function_argument(Expression arg)
{
	*arr_last(B.arities) += 1;
	arr_append(B.arguments, arg);
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

static void
B_extrn_register(Symbol sym)
{
	Scope	*current;

	if (arr_count(B.scopes) == 0)
		B_error(ERROR_SYMBOL, "no current scope for symbol.");

	current = arr_last(B.scopes);

	current->sym_count += 1;
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
B_extern_variable(StringC name)
{
	B_symbol_new(SYMBOL_EXTERN, name, WORD_SIZE);
	B_extrn_register(*arr_last(B.symbols));
}
