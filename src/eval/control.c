/**
* control.c
*/

#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include <b.h>

static StringC
B_label_name(LabelType type)
{
	B_DBG_TREE;

	static int	function_count = 0;
	static int	loop_count = 0;
	
	int			idx;
	char		buffer[128] = {0};
	StringC		format;
	StringC		formats[LABEL_XENUM_LAST] = 
	{
		[LABEL_FUNC_START ] = ".LFB%d_%d",
		[LABEL_FUNC_STOP  ] = ".LFE%d_%d",
		[LABEL_FUNC_RET   ] = ".LFR%d_%d",
		[LABEL_LOOP_START ] = ".LBB%d_%d",
		[LABEL_LOOP_STOP  ] = ".LBE%d_%d",
		[LABEL_SWITCH_SKIP] = ".LSB%d_%d",
		[LABEL_SWITCH_STOP] = ".LSE%d_%d",
		[LABEL_CASE_LOOKUP] = ".LCT%d_%d",
	};

	switch (type)
	{
		case LABEL_FUNC_START:
			function_count++;
		__attribute__((fallthrough));
		case LABEL_FUNC_STOP:
		__attribute__((fallthrough));
		case LABEL_FUNC_RET:
			idx = function_count;
			break;

		case LABEL_LOOP_START:
			idx = loop_count++;
		__attribute__((fallthrough));
		case LABEL_LOOP_STOP:
		case LABEL_SWITCH_SKIP:
		case LABEL_SWITCH_STOP:
		case LABEL_CASE_LOOKUP:
			idx = arr_count(B.labels.stacks[type]);
			break ;

		case LABEL_CASE:
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

static StringC
B_label_push(LabelType type)
{
	B_DBG_TREE;

	if (type >= LABEL_XENUM_LAST)
		B_compiler_error("invalid LabelType.");

	StringC	name = B_label_name(type);

	arr_append(B.labels.stacks[type], name);
	if (type == LABEL_LOOP_STOP || type == LABEL_SWITCH_STOP)
		arr_append(B.labels.context, name);

	B.labels.next++;
	return (name);
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

	arr_pop(B.labels.stacks[type], 1);
	if (type == LABEL_LOOP_STOP || type == LABEL_SWITCH_STOP)
		arr_pop(B.labels.context, 1);
	
	free((void *)name);
}

static StringC
B_label_last(LabelType type)
{
	B_DBG_TREE;

	if (type >= LABEL_XENUM_LAST)
		B_compiler_error("invalid LabelType.");

	if (arr_count(B.labels.stacks[type]) < 1)
		return (NULL);

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

void
B_control_function_start(StringC name)
{
	B_DBG_TREE;

	B_label_push(LABEL_FUNC_START);
	B_label_push(LABEL_FUNC_STOP);
	B_label_push(LABEL_FUNC_RET);
	
	CG_function(name);

	B_label(LABEL_FUNC_START);

#if defined(B_DEBUG)
	CG_debug_prolog();
#endif // B_DEBUG

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
	
	B_label(LABEL_FUNC_STOP);

#if defined(B_DEBUG)
	CG_debug_epilog();
#endif // B_DEBUG

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

	CG_expr_condition(e);
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

	CG_expr_condition(e);
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

void
B_control_break(void)
{
	B_DBG_TREE;

	if (arr_count(B.labels.context) == 0)
		B_compiler_error("'break' statement not in while or switch statement.");

	StringC	last = *arr_last(B.labels.context);
	StringC	last_loop = B_label_last(LABEL_LOOP_STOP);
	StringC	last_switch = B_label_last(LABEL_SWITCH_STOP);

	LabelType	lt = LABEL_XENUM_LAST;

	if (!last_loop)
		lt = LABEL_SWITCH_STOP;
	if (!last_switch)
		lt = LABEL_LOOP_STOP;
	if (lt == LABEL_XENUM_LAST)
	{
		if (!strcmp(last_loop, last))
			lt = LABEL_LOOP_STOP;
		if (!strcmp(last_switch, last))
			lt = LABEL_SWITCH_STOP;
	}
	B_label_jump(lt);
}

void
B_control_switch_start(Expression e)
{
	B_DBG_TREE;

	B_label_push(LABEL_SWITCH_SKIP);
	B_label_push(LABEL_SWITCH_STOP);
	B_label_jump(LABEL_SWITCH_SKIP);

	ExprAlloc	*alloc = arr_nth(EA, e);

	arr_append(B.labels.switchs, *alloc);
	
	Cases	cases = {0};
	
	arr_append(B.labels.cases, cases);
	B.labels.cases_in = arr_last(B.labels.cases);
}

static int
B_casecmp(const void *c1p, const void *c2p)
{
	const Case	*c1 = c1p;
	const Case	*c2 = c2p;

	return (c1->value - c2->value);
}

static bool
B_control_switch_range(void)
{
	B_DBG_TREE;

	i32	first = 0;
	i32	last = 0;

	qsort(arr_first(*B.labels.cases_in), arr_count(*B.labels.cases_in), sizeof(Case), B_casecmp);
	arr_foreach(Case, _case, *B.labels.cases_in)
	{
		if (!_case->label)
			continue ;
		if (!first++)
		{
			last = _case->value;
			continue;
		}
		if (_case->value != last + 1)
			return (false);
		last = _case->value;
	}
	return (true);
}

static void
B_control_switch_table(void)
{
	B_DBG_TREE;

	bool	first = false;
	StringC	table_name = B_label_push(LABEL_CASE_LOOKUP);
	
	CaseTable	table = 
	{
		.start = 0,
		.table_label = table_name,
		.case_labels = {0},
	};

	Expression	se = arr_count(EA);
	
	arr_append(EA, *arr_last(B.labels.switchs));

	arr_foreach(Case, _case, *B.labels.cases_in)
	{
		if (!first)
		{
			table.start = _case->value;
			first = true;
			CG_switch_lookup(se, table.start, table.table_label);
		}
		if (!_case->label)
			break ;
		arr_append(table.case_labels, _case->label);
	}
	arr_append(B.labels.tables, table);
}

static void
B_control_switch_linear(void)
{
	B_DBG_TREE;

	Expression	se = arr_count(EA);
	
	arr_append(EA, *arr_last(B.labels.switchs));

	ExprAlloc	*source = arr_last(EA);
	Case		default_case = {0};

	arr_foreach(Case, _case, *B.labels.cases_in)
	{
		if (!_case->label)
		{
			default_case = (Case)
			{
				.label = (StringC)_case->value,
				.value = 0,
			};
			continue ;
		}
		Expression	imm = EA_allocate_immediate(_case->value);
		
		source->data = (void *)(long)imm;
		CG_expr_condition(se);
		CG_jump_compare(_case->label, se);
	}
	if (default_case.label)
		CG_jump_label(default_case.label, BOP_NONE);
}

void
B_control_switch_stop(void)
{
	B_DBG_TREE;
	B_label(LABEL_SWITCH_SKIP);

	if (B_control_switch_range())
		B_control_switch_table();
	else
		B_control_switch_linear();

	arr_destroy(*B.labels.cases_in);
	arr_pop(B.labels.switchs, 1);
	arr_pop(B.labels.cases, 1);

	B.labels.cases_in = arr_last(B.labels.cases);

	B_label(LABEL_SWITCH_STOP);
	B_label_pop(LABEL_SWITCH_SKIP);
	B_label_pop(LABEL_SWITCH_STOP);
}

static void
B_control_switch_case(StringC lbl, i64 value)
{
	Case	_case = 
	{
		.label = lbl,
		.value = value,
	};
	arr_append(*B.labels.cases_in, _case);
	B_label(LABEL_CASE);
}

void
B_control_switch_case_const(Expression e)
{
	B_DBG_TREE;

	StringC	name = B_label_push(LABEL_CASE);

	ExprAlloc	*alloc = arr_nth(EA, e);

	if (alloc->op.type != OPERAND_IMMEDIATE)
		unreachable("nullos va");

	B_control_switch_case(name, alloc->op.imm);
}

void
B_control_switch_default(void)
{
	B_DBG_TREE;

	StringC	name = B_label_push(LABEL_CASE);

	B_control_switch_case(NULL, (i64)name);
}
