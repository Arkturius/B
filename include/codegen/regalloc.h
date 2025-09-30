/**
* regalloc.h
*/

#if !defined (_B_REGALLOC_H)
# define _B_REGALLOC_H

# include <codegen/emission.h>

typedef struct b_reg_allocation	RegAlloc;

x_enum 
(
	RegClass,
	x_enum_prefix(REG_CLASS),
	x_enum_members
	(
		(ANY     ),
		(ACCUM   ),
		(NOT_A   ),
		(NOT_D   ),
		(SPECIFIC),
	)
)

x_enum
(
	RegStatus,
	x_enum_prefix(REG_STATUS),
	x_enum_members 
	(
		(FREE     ),
		(ALLOCATED),
		(RESERVED ),
		(SPILLED  ),
	)
)

struct b_reg_allocation
{
	x86Operand	op;

	RegStatus   status;
	bool		dirty;
	bool		pinned;

	u32			last_use;
	void		*data;
};

x_array(struct b_reg_allocation, RegAllocator);

extern RegAllocator	RA;

#endif // _B_REGALLOC_H
