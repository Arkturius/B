/**
* control.h - Control flow handling.
*/

#if !defined (_B_CONTROL_H)
# define _B_CONTROL_H

# define XLIB_NO_PREFIX
# include <xlib.h>

typedef u32	LabelID;

x_enum 
(
	LabelType,
	x_enum_prefix(LABEL),
	x_enum_members
	(
		(FUNCTION_END),
		(LOOP_START  ),
		(LOOP_STOP   ),
		(SKIP_IF     ),
		(SKIP_ELSE   )
	)
)

x_array(StringC, LabelStack);

typedef struct	b_label_frame
{
	LabelStack	stacks[LABEL_XENUM_LAST];
	LabelID		next;
}	LabelFrame;

void
B_control_program_start(void);

void
B_control_program_stop(void);

void
B_control_function_start(StringC name);

void
B_control_function_stop(void);

#endif // _B_CONTROL_H
