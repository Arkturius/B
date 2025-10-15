/**
* control.h - Control flow handling.
*/

#if !defined (_B_CONTROL_H)
# define _B_CONTROL_H

# define XLIB_NO_PREFIX
# include <xlib.h>

# include <eval/expression.h>
# include <codegen/regalloc.h>

typedef u32	LabelID;

x_enum 
(
	LabelType,
	x_enum_prefix(LABEL),
	x_enum_members
	(
		(FUNC_START  ),
		(FUNC_STOP   ),
		(FUNC_RET    ),
		(LOOP_START  ),
		(LOOP_STOP   ),
		(SKIP_IF     ),
		(SKIP_ELSE   ),
		(SWITCH_SKIP ),
		(SWITCH_STOP ),
		(CASE        ),
		(CASE_LOOKUP ),
	)
)

x_array(StringC, LabelStack);

typedef struct	b_case
{
	StringC	label;
	i64		value;
}	Case;

x_array(Case, Cases);
x_array(Cases, CaseStack);

typedef struct	b_table
{
	Offset		start;
	StringC		table_label;
	LabelStack	case_labels;
}	CaseTable;

x_array(CaseTable, CaseTables);

typedef struct	b_label_frame
{
	LabelStack		stacks[LABEL_XENUM_LAST];
	LabelStack		context;

	CaseStack		cases;
	Cases			*cases_in;
	ExprAllocator	switchs;
	CaseTables		tables;

	LabelID			next;
}	LabelFrame;

void
B_control_program_start(void);

void
B_control_program_stop(void);

void
B_control_function_start(StringC name);

void
B_control_function_stop(void);

void
B_control_while_start(void);

void
B_control_while_test(Expression e);

void
B_control_while_stop(void);

void
B_control_if_start(Expression e);

void
B_control_if_stop(bool has_else);

void
B_control_else_stop(void);


void
B_control_return(Expression e);

#endif // _B_CONTROL_H
