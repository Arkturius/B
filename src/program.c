/**
 * program.c
 */

#include "arr.h"
#include <b.h>
#include <codegen.h>

static void
B_rodata()
{
	emit_directive(DIRECTIVE_SECTION, .data = SECTION(SECTION_RODATA));
	arr_foreach(RoString, rostr, B.rostrings)
	{
		emit_label(rostr->name);
		emit_directive(DIRECTIVE_LONG, .data = rostr->name, .off = WORD_SIZE);
		emit_directive(DIRECTIVE_STRING, .data = rostr->text);
		printf("\n");
	}
}

void
B_program_start(void)
{
	emit_directive(DIRECTIVE_SYNTAX,  .data = SYNTAX(SYNTAX_INTEL));
	emit_directive(DIRECTIVE_SECTION, .data = SECTION(SECTION_TEXT));
	printf("\n");
}

void
B_program_stop(void)
{
	B_rodata();
}

void
B_label_push(LabelType type)
{
	if (type >= LABEL_ENUM_MAX)
		B_error(ERROR_SYNTAX, "invalid label type.");

	Label	label;

	label.id	= B.labels.next_id++;
	label.name	= B_asprintf(".L%d", label.id);

	arr_append(B.labels.grid[type], label);
}

void
B_label_pop(LabelType type)
{
	if (type >= LABEL_ENUM_MAX)
		B_error(ERROR_SYNTAX, "invalid label type.");

	arr_pop(B.labels.grid[type], 1);
}

Label
B_label_get(LabelType type)
{
	if (type >= LABEL_ENUM_MAX)
		B_error(ERROR_SYNTAX, "invalid label type.");

	if (arr_count(B.labels.grid[type]) == 0)
		B_error(ERROR_SYNTAX, "no existing label for this type.");

	return (*arr_last(B.labels.grid[type]));
}

void
B_scope_start(void)
{
	Scope	*current = arr_last(B.scopes);

	Scope	new = 
	{
		.decl_size	= 0,
		.sym_count	= 0,
		.sym_start	= arr_count(B.symbols),
		.stack		= current ? current->stack : 0,
	};
	arr_append(B.scopes, new);
}

void
B_scope_stop(void)
{
	Scope	*current = arr_last(B.scopes);

	if (arr_count(B.scopes) == 0)
		B_error(ERROR_SYNTAX, "trying to exit global scope.");

	arr_pop(B.symbols, current->sym_count);
	arr_pop(B.scopes, 1);
}

void
B_return_expr(Expression ret)
{
	if (ret.type != EXPR_REGISTER || ret.reg != REG_NULL)
		code_move(EAX, ret);
	code_jump(BINOP, LABEL_FUNC_STOP, NULL);
}

void
B_while_start(void)
{
	B_label_push(LABEL_LOOP_START);
	B_label_push(LABEL_LOOP_STOP);

	code_label(LABEL_LOOP_START);
}

void
B_while_condition(Expression cond)
{
	code_jump(cond.comparison, LABEL_LOOP_STOP, NULL);
}

void
B_while_stop(void)
{
	code_jump(BINOP, LABEL_LOOP_START, NULL);
	code_label(LABEL_LOOP_STOP);

	B_label_pop(LABEL_LOOP_START);
	B_label_pop(LABEL_LOOP_STOP);
}

void
B_break(void)
{
	if (arr_count(B.labels.grid[LABEL_LOOP_START]))
		B_error(ERROR_SYNTAX, "break outside of a while loop.");

	code_jump(BINOP, LABEL_LOOP_STOP, NULL);
}

void
B_goto(StringC name)
{
}
