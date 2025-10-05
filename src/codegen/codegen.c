/**
* codegen.c
*/

#include "codegen/codegen.h"
#include "codegen/emission.h"
#include "codegen/regalloc.h"
#include <b.h>

void
CG_program_header(void)
{
	ASM_dir_syntax();
}

void
CG_switch_section(SectionType section)
{
	if (section == SECTION_NONE || section >= SECTION_XENUM_LAST)
		B_compiler_error("invalid SectionType.");

	if (B.section != section)
		ASM_dir_section(section);
	B.section = section;
}

void
CG_rodata_section(void)
{
	if (arr_count(B.rostrings) == 0)
		return ;

	CG_switch_section(SECTION_RODATA);

	todo("dump all rostrings to form .rodata section");
}

void
CG_data_scalar_list(void)
{
	CG_switch_section(SECTION_DATA);

	ASM_dir_long(NULL, &B.ivals);
	arr_count(B.ivals) = 0;
}

void
CG_data_vector(Size size)
{
	CG_switch_section(SECTION_DATA);

	if (arr_count(B.ivals) < size)
	{
		arr_realloc(B.ivals, size);
		memset(arr_last(B.ivals) + 1, 0, sizeof(IVal) * (size - arr_count(B.ivals)));
		arr_count(B.ivals) = size;
	}
	ASM_dir_long(NULL, &B.ivals);
	arr_count(B.ivals) = 0;
}

void
CG_data_section(void)
{
	if (arr_count(B.internals) == 0)
		return ;

	CG_switch_section(SECTION_DATA);

	arr_foreach(Symbol, internal, B.internals)
	{
		arr_append(B.ivals, (IVal){0});

		CG_label(internal->name, false);
		CG_data_scalar_list();
	}
	arr_count(B.internals) = 0;
}

void
CG_label(StringC label, bool user)
{
	ASM_label(label);
	if (user)
		ASM_dir_long(label, WORD_SIZE);
}

void
CG_align(Size align)
{
	ASM_dir_align(align);
}

void
CG_function(StringC name)
{
	printf("\n");
	CG_switch_section(SECTION_TEXT);
	CG_align(16);
	CG_label(name, true);
}

void
CG_prolog(void)
{
	x86Operand	ebp = REG_OPERAND(X86_REG_EBP);
	x86Operand	esp = REG_OPERAND(X86_REG_ESP);

	ASM_push(ebp);
	ASM_mov(ebp, esp);
}

void
CG_epilog(void)
{
	x86Operand	ebp = REG_OPERAND(X86_REG_EBP);

	ASM_pop(ebp);
	ASM_ret();
	printf("\n");
}

void
CG_return(Expression e)
{
	x86Operand	eax = REG_OPERAND(X86_REG_EAX);

//	ASM_mov(eax, ret);
}


void
CG_stack_reserve(Size size)
{
	x86Operand	esp = REG_OPERAND(X86_REG_ESP);

	ASM_sub(esp, IMM_OPERAND(size));
}

void
CG_stack_release(Size size)
{
	x86Operand	esp = REG_OPERAND(X86_REG_ESP);

	ASM_add(esp, IMM_OPERAND(size));
}

x86Operand
CG_expr_lvalue(Expression e)
{
	if (e >= arr_count(EA))
		B_compiler_error("out of bounds Expression.");

	ExprAlloc	*ex = arr_nth(EA, e);

	switch (ex->op.type)
	{
		case OPERAND_IMMEDIATE:
			B_compiler_error("immediate as lvalue.");
		case OPERAND_REGISTER:
		case OPERAND_MEMORY:
		case OPERAND_SYMBOL:
			return ex->op;
		default:
			unreachable("%s: invalid OperandType", __func__);
	}
}

x86Operand
CG_expr_rvalue(Expression e)
{
	
}

void
CG_operand_constraints(x86Operand *dst, x86Operand *src)
{
	if (dst->type == OPERAND_MEMORY && src->type == OPERAND_MEMORY)
	{
		x86Operand	scratch = REG_OPERAND(RP_register_alloc(REG_CLASS_ANY));

		ASM_mov(scratch, *src);
		*src = scratch;
	}
}

void
CG_move(Expression dst, Expression src)
{
	x86Operand	op_dst = CG_expr_lvalue(dst);
	x86Operand	op_src = CG_expr_rvalue(src);

	CG_operand_constraints(&op_dst, &op_src);
	ASM_mov(op_dst, op_src);
	EA_expr_update(dst, op_dst, EXPR_STATUS_ALLOCATED);
	arr_delete(EA, src);

	asm("int3");
}

