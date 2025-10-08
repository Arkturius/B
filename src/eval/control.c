/**
* control.c
*/

#include "eval/control.h"
#include "codegen/regalloc.h"
#include <assert.h>
#include <string.h>

#include <b.h>

static StringC
B_label_name(LabelType type)
{
	B_DBG_TREE;

	static int	function_count = 0;
	static int	block_count = 0;
	
	int			idx;
	char		buffer[128] = {0};
	StringC		format;
	StringC		formats[LABEL_XENUM_LAST] = 
	{
		[LABEL_FUNC_START] = ".LFB%d_%d",
		[LABEL_FUNC_STOP]  = ".LFE%d_%d",
		[LABEL_FUNC_RET]   = ".LFR%d_%d",
		[LABEL_LOOP_START] = ".LBB%d_%d",
		[LABEL_LOOP_STOP]  = ".LBE%d_%d",
	};

	switch (type)
	{
		case LABEL_FUNC_START:
			function_count++;
			/* fallthrough */
		case LABEL_FUNC_STOP:
			/* fallthrough */
		case LABEL_FUNC_RET:
			idx = function_count;
			break;
		case LABEL_LOOP_START:
			idx = block_count++;
			/* fallthrough */
		case LABEL_LOOP_STOP:
			idx = arr_count(B.labels.stacks[type]);
			break ;
		case LABEL_SKIP_IF:
		case LABEL_SKIP_ELSE:
			snprintf(buffer, sizeof(buffer), ".L%d", B.labels.next);
			return (StringC) strdup(buffer);
		default:
			B_compiler_error("invalid LabelType.");
	}
	format = formats[type];
	snprintf(buffer, sizeof(buffer), format, function_count - 1, idx);

	return (StringC) strdup(buffer);
}

static void
B_label_push(LabelType type)
{
	B_DBG_TREE;

	if (type >= LABEL_XENUM_LAST)
		B_compiler_error("invalid LabelType.");

	StringC	name = B_label_name(type);

	arr_append(B.labels.stacks[type], name);
	B.labels.next++;
}

static void
B_label_pop(LabelType type)
{
	B_DBG_TREE;

	if (type >= LABEL_XENUM_LAST)
		B_compiler_error("invalid LabelType.");

	if (arr_count(B.labels.stacks[type]) < 1)
		B_compiler_error("no label to pop for this type.");

	StringC	name = *arr_last(B.labels.stacks[type]);

	free((void *)name);
	arr_pop(B.labels.stacks[type], 1);
}

static StringC
B_label_last(LabelType type)
{
	B_DBG_TREE;

	if (type >= LABEL_XENUM_LAST)
		B_compiler_error("invalid LabelType.");

	if (arr_count(B.labels.stacks[type]) < 1)
		B_compiler_error("no label to pop for this type.");

	StringC	name = *arr_last(B.labels.stacks[type]);

	return (name);
}

static void
B_label_jump(LabelType type)
{
	B_DBG_TREE;

	if (type >= LABEL_XENUM_LAST)
		B_compiler_error("invalid LabelType.");

	if (arr_count(B.labels.stacks[type]) < 1)
		B_compiler_error("no label to jump to for this type.");

	StringC	name = *arr_last(B.labels.stacks[type]);

	CG_jump_label(name, BOP_NONE);
}

static void
B_label(LabelType type)
{
	B_DBG_TREE;

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
	B_DBG_TREE;

	B_scope_enter();

	CG_program_header();
}

void
B_control_program_stop(void)
{
	B_DBG_TREE;

	assert(arr_count(B.scopes) == 1 && "program can't finish outside of main scope.");
}

static void
CG_debug_prolog()
{
	B_DBG_TREE;

	B_label(LABEL_FUNC_START);

#if defined(B_DEBUG)
	ASM_dir_file();
	ASM_dir_loc("");
	ASM_dir_cfi_sp();
#endif // B_DEBUG

}

static void
CG_debug_epilog()
{
	B_DBG_TREE;

	B_label(LABEL_FUNC_STOP);

#if defined(B_DEBUG)
	StringC	func_end = *arr_last(B.labels.stacks[LABEL_FUNC_STOP]);
	StringC	size_dir = B_asprintf("%s, %s - %s", B.function_name, func_end, B.function_name);

	ASM_dir_size(size_dir);
	ASM_dir_cfi_ep();

	free((String) size_dir);
#endif // B_DEBUG

}

void
B_control_function_start(StringC name)
{
	B_DBG_TREE;

	B_label_push(LABEL_FUNC_START);
	B_label_push(LABEL_FUNC_STOP);
	B_label_push(LABEL_FUNC_RET);
	
	CG_function(name);

	CG_debug_prolog();
	CG_prolog();

	B_scope_enter();
	B.function_name = name;
}

void
B_control_function_stop(void)
{
	B_DBG_TREE;

	B_label(LABEL_FUNC_RET);

	CG_epilog();
	CG_debug_epilog();

	CG_rodata_section();
	CG_data_section();

	B_label_pop(LABEL_FUNC_START);
	B_label_pop(LABEL_FUNC_STOP);
	B_label_pop(LABEL_FUNC_RET);
	
	B_scope_leave();
	B.function_name = NULL;
}

void
B_control_return(Expression e)
{
	B_DBG_TREE;

	CG_return(e);
	B_label_jump(LABEL_FUNC_RET);
}

void
B_control_while_start(void)
{
	B_DBG_TREE;

	B_label_push(LABEL_LOOP_START);
	B_label_push(LABEL_LOOP_STOP);

	B_label(LABEL_LOOP_START);
}

void
B_control_while_test(Expression e)
{
	B_DBG_TREE;

	StringC	end = B_label_last(LABEL_LOOP_STOP);

	if (EA_get_data(e) == BOP_NONE)
		CG_test(e);
	CG_jump_compare(end, e);
}

void
B_control_while_stop(void)
{
	B_DBG_TREE;

	B_label_jump(LABEL_LOOP_START);
	B_label(LABEL_LOOP_STOP);

	B_label_pop(LABEL_LOOP_START);
	B_label_pop(LABEL_LOOP_STOP);
}

void
B_control_if_start(Expression e)
{
	B_DBG_TREE;

	B_label_push(LABEL_SKIP_IF);
	
	StringC	skip = B_label_last(LABEL_SKIP_IF);

	if (EA_get_data(e) == BOP_NONE)
		CG_test(e);
	CG_jump_compare(skip, e);
}

void
B_control_if_stop(bool has_else)
{
	B_DBG_TREE;

	if (has_else)
	{
		B_label_push(LABEL_SKIP_ELSE);
		B_label_jump(LABEL_SKIP_ELSE);
	}
	B_label(LABEL_SKIP_IF);
	B_label_pop(LABEL_SKIP_IF);
}

void
B_control_else_stop(void)
{
	B_DBG_TREE;

	B_label(LABEL_SKIP_ELSE);
	B_label_pop(LABEL_SKIP_ELSE);
}



