/**
* control.c
*/

#include "codegen/codegen.h"
#include "symbols.h"
#include <assert.h>
#include <string.h>

#include <b.h>

static StringC
B_label_name(void)
{
	char	buffer[128] = {0};

	snprintf(buffer, sizeof(buffer), ".L%d", B.labels.next);

	return (StringC) strdup(buffer);
}

static void
B_label_push(LabelType type)
{
	if (type >= LABEL_XENUM_LAST)
		B_compiler_error("invalid LabelType.");

	StringC	name = B_label_name();

	arr_append(B.labels.stacks[type], name);
	B.labels.next++;
}

static void
B_label_pop(LabelType type)
{
	if (type >= LABEL_XENUM_LAST)
		B_compiler_error("invalid LabelType.");

	if (arr_count(B.labels.stacks[type]) < 1)
		B_compiler_error("no label to pop for this type.");

	StringC	name = *arr_last(B.labels.stacks[type]);

	free((void *)name);
	arr_pop(B.labels.stacks[type], 1);
}

static void
B_label(LabelType type)
{
	if (type >= LABEL_XENUM_LAST)
		B_compiler_error("invalid LabelType.");

	if (arr_count(B.labels.stacks[type]) < 1)
		B_compiler_error("no label to emit for this type.");

	StringC	name = *arr_last(B.labels.stacks[type]);

	CG_label(name, false);
}

void
B_control_program_start(void)
{
	B_scope_enter();

	CG_program_header();
}

void
B_control_program_stop(void)
{
	assert(arr_count(B.scopes) == 1 && "program can't finish outside of main scope.");
}

void
B_control_function_start(StringC name)
{
	B_label_push(LABEL_FUNCTION_END);
	
	CG_function(name);
	CG_prolog();

	B_scope_enter();

	B.function_name = name;
}

void
B_control_function_stop(void)
{
	B_label(LABEL_FUNCTION_END);
	B_label_pop(LABEL_FUNCTION_END);

	CG_epilog();
	CG_rodata_section();
	CG_data_section();

	B_scope_leave();

	B.function_name = NULL;
}
