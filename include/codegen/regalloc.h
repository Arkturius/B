/**
* regalloc.h
*/

#if !defined (_B_REGALLOC_H)
# define _B_REGALLOC_H

# include <symbols.h>
# include <codegen/emission.h>

typedef struct b_expr_allocation	ExprAlloc;

x_enum
(
	ExprStatus,
	x_enum_prefix(EXPR_STATUS),
	x_enum_members 
	(
		(FREE     ),
		(ALLOCATED),
		(RESERVED ),
		(COMPARE  ),
		(SPILLED  ),
	)
)

struct b_expr_allocation
{
	x86Operand	op;
	ExprStatus  status;
	void		*data;
};

x_array(struct b_expr_allocation, ExprAllocator);

extern ExprAllocator	EA;

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

typedef struct b_reg_state
{
	Expression	owner;
	bool		used;
	bool		dirty;
	u32			tick;
}	RegState;

x_array(RegState, RegStates);

typedef struct b_reg_pool
{
	RegStates	states;
	u32			tick;
}	RegPool;

extern RegPool	RP;

x86Register
RP_register_alloc(RegClass hint);

void
RP_register_free(x86Register reg);


void
*EA_get_data(Expression e);

Expression
EA_allocate_symbol(Symbol *symbol);

Expression
EA_allocate_comparison(BOpType type, Expression a, Expression b);

Expression
EA_allocate_string(StringC name);

Expression
EA_allocate_immediate(i32 imm);

Expression
EA_expr_copy(Expression e);

void
EA_expr_update(Expression e, x86Operand op, ExprStatus status);

void
EA_expr_cleanup(void);

#endif // _B_REGALLOC_H
