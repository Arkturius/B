/**
* regalloc.c
*/

#include "codegen/emission.h"
#include "eval/expression.h"
#include <b.h>
#include <codegen/regalloc.h>

ExprAllocator	EA = {0};
RegPool			RP = {0};

static Expression
EA_allocate_auto(Symbol *symbol)
{
	switch (symbol->vtype)
	{
		case VARIABLE_SCALAR:
		case VARIABLE_VECTOR:
		case VARIABLE_UNKNOWN:
		{
			Expression	e = arr_count(EA);
			ExprAlloc	alloc = 
			{
				.status = EXPR_STATUS_RESERVED,
				.op = MEM_OPERAND(X86_MEM
				(
					.base         = X86_REG_EBP,
					.displacement = symbol->off * WORD_SIZE
				))
			};
			arr_append(EA, alloc);
			return (e);
		}
		default:
			break ;
	}
	unreachable("%s: invalid variable type for AUTO symbol.", __func__);
}

static Expression
EA_allocate_extern(Symbol *symbol)
{
	todo("%s", __func__);
}

static Expression
EA_allocate_intern(Symbol *symbol)
{
	todo("%s", __func__);
}

Expression
EA_allocate_symbol(Symbol *symbol)
{
	switch (symbol->stype)
	{
		case STORAGE_AUTO:
			return (EA_allocate_auto(symbol));
		case STORAGE_EXTERN:
			return (EA_allocate_extern(symbol));
		case STORAGE_INTERN:
			return (EA_allocate_intern(symbol));
		default:
			break ;
	}
	unreachable("%s: invalid StorageType", __func__);
}

static x86RegisterBase
RP_register_alloc_accum(void)
{
	if (!arr_nth(RP.states, X86_BASE_A)->used)
		return (X86_BASE_A);
	return (0);
}

static x86RegisterBase
RP_register_alloc_any(void)
{
	x86RegisterBase	bases[] =
	{
		X86_BASE_D, X86_BASE_A,  X86_BASE_C,
		X86_BASE_B, X86_BASE_DI, X86_BASE_SI,
	};

	for (u32 i = 0; i < array_len(bases); ++i)
	{
		x86RegisterBase	base = bases[i];

		if (!arr_nth(RP.states, base)->used)
			return (base);
	}
	return (0);
}

static x86RegisterBase
RP_register_alloc_not(x86RegisterBase excl)
{
	x86RegisterBase	base;

	for (base = X86_BASE_A; base < X86_BASE_XENUM_LAST; ++base)
	{
		if (base == X86_BASE_BP || base == X86_BASE_SP)
			continue ;
		if (base == excl)
			continue ;
		if (!arr_nth(RP.states, base)->used)
			return (base);
	}
	return (0);
}

void
RP_init(void)
{
	RegState	empty = {0};

	arr_reserve(RP.states, X86_BASE_XENUM_LAST);
	for (x86RegisterBase base = 0; base < X86_BASE_XENUM_LAST; ++base)
		arr_append(RP.states, empty);
}

void
RP_register_writeback(x86Register reg)
{
    x86RegisterBase base = X86_REG_BASE(reg);
	RegState		*state = arr_nth(RP.states, base);

    if (state->dirty)
        return;
        
    Expression owner = state->owner;
    if (!owner)
		return;
    
    ExprAlloc	*alloc = arr_nth(EA, owner);
	x86Operand	op = alloc->op;
    
    ASM_mov(op, REG_OPERAND(reg));
    state->dirty = false;
}

x86RegisterBase
RP_register_spill(RegClass hint)
{
    x86RegisterBase victim = X86_BASE_A;
    u32				oldest = UINT32_MAX;
    
    for (x86RegisterBase base = X86_BASE_A; base < X86_BASE_XENUM_LAST; ++base)
	{
        if (base == X86_BASE_SP || base == X86_BASE_BP)
			continue;
        if (hint == REG_CLASS_NOT_A && base == X86_BASE_A)
			continue;
        if (hint == REG_CLASS_NOT_D && base == X86_BASE_D)
			continue;
        if (hint == REG_CLASS_ACCUM && base != X86_BASE_A)
			continue;
        
		RegState	*state = arr_nth(RP.states, base);
        if (state->used && state->tick < oldest)
		{
            oldest = state->tick;
            victim = base;
        }
    }
    
    x86Register victim_reg = X86_REG_MAKE(victim, X86_SIZE_DWORD);
    RP_register_writeback(victim_reg);
    
    Expression victim_expr = arr_nth(RP.states, victim)->owner;
    if (victim_expr != EXPR_INVALID)
        arr_nth(EA, victim_expr)->status = EXPR_STATUS_SPILLED;
    
    arr_nth(RP.states, victim)->used = false;
    return victim;
}

x86Register
RP_register_alloc(RegClass hint)
{
	x86RegisterBase	base;

	switch (hint)
	{
		case REG_CLASS_ACCUM:
			base = RP_register_alloc_accum();
			if (base)
				goto found;
			__attribute__((fallthrough));
		default:
		case REG_CLASS_ANY:
			base = RP_register_alloc_any();
			if (base)
				goto found;
			break ;
		case REG_CLASS_NOT_A:
			base = RP_register_alloc_not(X86_BASE_A);
			if (base)
				goto found;
			break ;
		case REG_CLASS_NOT_D:
			base = RP_register_alloc_not(X86_BASE_D);
			if (base)
				goto found;
			break ;
	}
	base = RP_register_spill(hint);
    
found:
    arr_nth(RP.states, base)->used = true;
	arr_nth(RP.states, base)->tick = RP.tick++;
    
	return (X86_REG_MAKE(base, X86_SIZE_DWORD));
}

void
RP_register_free(x86Register reg)
{
	x86RegisterBase	base = X86_REG_BASE(reg);

	if (base >= X86_BASE_XENUM_LAST)
		B_compiler_error("invalid register base.");

	RP_register_writeback(reg);

	RegState	*state = arr_nth(RP.states, base);

	state->used = false;
	state->owner = 0;
	state->dirty = false;
}

void
EA_expr_update(Expression e, x86Operand op, ExprStatus status)
{
	if (e >= arr_count(EA))
		B_compiler_error("invalid Expression.");

	ExprAlloc	*expr = arr_nth(EA, e);

	expr->op = op;
	expr->status = status;
}

Expression
EA_allocate_register(void)
{
	Expression	e = arr_count(EA);

	
}
