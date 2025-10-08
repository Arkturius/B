/**
* codegen.c
*/

#include "codegen/emission.h"
#include "codegen/regalloc.h"
#include "eval/expression.h"
#include "xlib.h"
#include <b.h>

void
CG_program_header(void)
{
	B_DBG_TREE;

	ASM_dir_syntax();

#if defined(B_DEBUG)
	ASM_dir_file();
#endif // B_DEBUG

}

void
CG_switch_section(SectionType section)
{
	B_DBG_TREE;

	if (section == SECTION_NONE || section >= SECTION_XENUM_LAST)
		B_compiler_error("invalid SectionType.");

	if (B.section != section)
		ASM_dir_section(section);
	B.section = section;
}

void
CG_rodata_section(void)
{
	B_DBG_TREE;

	if (arr_count(B.rostrings) == 0)
		return ;

	CG_switch_section(SECTION_RODATA);

	todo("dump all rostrings to form .rodata section");
}

void
CG_data_scalar_list(void)
{
	B_DBG_TREE;

	CG_switch_section(SECTION_DATA);

	ASM_dir_long(NULL, &B.ivals);
	arr_count(B.ivals) = 0;
}

void
CG_data_vector(Size size)
{
	B_DBG_TREE;

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
	B_DBG_TREE;

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
	B_DBG_TREE;

	ASM_label(label);
	if (user)
		ASM_dir_long(label, WORD_SIZE);
}

void
CG_function(StringC name)
{
	B_DBG_TREE;

	printf("\n");
	CG_switch_section(SECTION_TEXT);

	ASM_dir_global(name);
	ASM_dir_palign(4);

#if defined(B_DEBUG)
	ASM_dir_type(name);
#endif // B_DEBUG

	CG_label(name, true);
}

void
CG_prolog(void)
{
	B_DBG_TREE;

	x86Operand	ebp = REG_OPERAND(X86_REG_EBP);
	x86Operand	esp = REG_OPERAND(X86_REG_ESP);
	x86Operand	eax = REG_OPERAND(X86_REG_EAX);

	ASM_push(ebp);
	ASM_mov(ebp, esp);
	ASM_xor(eax, eax);
}

void
CG_epilog(void)
{
	B_DBG_TREE;

	x86Operand	ebp = REG_OPERAND(X86_REG_EBP);
	x86Operand	esp = REG_OPERAND(X86_REG_ESP);

	ASM_mov(esp, ebp);
	ASM_pop(ebp);
	ASM_ret();
}

void
CG_stack_reserve(Size size)
{
	B_DBG_TREE;

	x86Operand	esp = REG_OPERAND(X86_REG_ESP);

	ASM_sub(esp, IMM_OPERAND(size));
}

void
CG_stack_release(Size size)
{
	B_DBG_TREE;

	x86Operand	esp = REG_OPERAND(X86_REG_ESP);

	ASM_add(esp, IMM_OPERAND(size));
}

x86Operand
CG_expr_lvalue(Expression e)
{
	B_DBG_TREE;

	if (e >= arr_count(EA))
	{
		todo("handle this.");
		B_compiler_error("out of bounds Expression.");
	}

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
	B_DBG_TREE;

	if (e >= arr_count(EA))
	{
		todo("handle this.");
		B_compiler_error("out of bounds Expression.");
	}

	ExprAlloc	*ex = arr_nth(EA, e);

	switch (ex->op.type)
	{
		case OPERAND_IMMEDIATE:
		case OPERAND_REGISTER:
		case OPERAND_MEMORY:
		case OPERAND_SYMBOL:
			return ex->op;
		default:
			unreachable("%s: invalid OperandType", __func__);
	}
	return (x86Operand){0};
}

static void
CG_operand_constraints(x86Operand *dst, x86Operand *src)
{
	B_DBG_TREE;

	if (dst->type == OPERAND_MEMORY && (src->type == OPERAND_MEMORY || src->type == OPERAND_SYMBOL))
	{
		x86Operand	scratch = REG_OPERAND(RP_register_alloc(REG_CLASS_ANY));

		ASM_mov(scratch, *src);
		*src = scratch;
	}
}

void
CG_return(Expression e)
{
	B_DBG_TREE;

	x86Operand	eax = REG_OPERAND(X86_REG_EAX);
	x86Operand	ret;

	if (e != EXPR_INVALID)
		ret = CG_expr_rvalue(e);
	else
		ret = IMM_OPERAND(0);

	ASM_mov(eax, ret);
}

void
CG_move(Expression dst, Expression src)
{
	B_DBG_TREE;

	x86Operand	op_dst = CG_expr_lvalue(dst);
	x86Operand	op_src = CG_expr_rvalue(src);

	CG_operand_constraints(&op_dst, &op_src);

	if (op_dst.type == OPERAND_MEMORY && op_src.type != OPERAND_REGISTER)
		op_dst.mem.size = X86_SIZE_DWORD;

	ASM_mov(op_dst, op_src);
	EA_expr_update(dst, op_dst, EXPR_STATUS_ALLOCATED);

	if (op_src.type == OPERAND_REGISTER)
		RP_register_free(op_src.reg);

	arr_delete(EA, src);
}

typedef void	(*FN_asm_binop)(x86Operand, x86Operand);

void
CG_binop(BOpType op, Expression a, Expression b)
{
	B_DBG_TREE;

	static const FN_asm_binop	binop_functions[BOP_XENUM_LAST] = 
	{
		[BOP_PLUS ] = ASM_add,
		[BOP_MINUS] = ASM_sub,
	};
	// get rid of switch case when all binop are implemented
	switch (op)
	{
		case BOP_MINUS:
		case BOP_PLUS:
			break ;
		default:
			todo("%s", __func__);
	}
	x86Operand	acc  = REG_OPERAND(RP_register_alloc(REG_CLASS_ACCUM));
	x86Operand	op_a = CG_expr_rvalue(a);
	x86Operand	op_b = CG_expr_rvalue(b);

	if (op_a.type == OPERAND_MEMORY)
		op_a.mem.size = X86_SIZE_DWORD;

	ASM_mov(acc, op_a);
	binop_functions[op](acc, op_b);
	EA_expr_update(a, acc, EXPR_STATUS_ALLOCATED);

	if (op_a.type == OPERAND_REGISTER)
		RP_register_free(op_a.reg);
	if (op_b.type == OPERAND_REGISTER)
		RP_register_free(op_b.reg);
}

void
CG_compare(Expression a, Expression b)
{
	B_DBG_TREE;

	x86Operand	op_a = CG_expr_rvalue(a);
	x86Operand	op_b = CG_expr_rvalue(b);

	CG_operand_constraints(&op_a, &op_b);
	if (op_a.type == OPERAND_MEMORY)
		op_a.mem.size = X86_SIZE_DWORD;

	ASM_cmp(op_a, op_b);

	if (op_a.type == OPERAND_REGISTER)
		RP_register_free(op_a.reg);
	if (op_b.type == OPERAND_REGISTER)
		RP_register_free(op_b.reg);
}

void
CG_test(Expression e)
{
	B_DBG_TREE;

	x86Operand	op_e = CG_expr_rvalue(e);

	switch (op_e.type)
	{
		case OPERAND_REGISTER:
		case OPERAND_IMMEDIATE:
			break ;
		case OPERAND_MEMORY:
			CG_operand_constraints(&op_e, &op_e);
			break ;
		default:
			B_compiler_error("invalid OperandType for 'test'.");
	}
	ASM_test(op_e, op_e);
}

typedef void	(*FN_asm_jump)(x86Operand);

void
CG_jump_label(StringC lbl, BOpType cond)
{
	B_DBG_TREE;

	FN_asm_jump	jump_functions[BOP_XENUM_LAST] = 
	{
		[BOP_NONE] = ASM_jmp,
		[BOP_EQ  ] = ASM_jne,
		[BOP_NE  ] = ASM_je,
		[BOP_LT  ] = ASM_jge,
		[BOP_LE  ] = ASM_jg,
		[BOP_GT  ] = ASM_jle,
		[BOP_GE  ] = ASM_jl,
	};
	switch (cond)
	{
		case BOP_NONE:
		case BOP_EQ:
		case BOP_NE:
		case BOP_GT:
		case BOP_GE:
		case BOP_LT:
		case BOP_LE:
			break ;
		default:
			todo("more conditions ! (%s)", x_tostr_BOpType(cond));
	}
	x86Operand	jump = SYM_OPERAND(lbl, .internal = true);

	jump_functions[cond](jump);
}

void
CG_jump_compare(StringC lbl, Expression e)
{
	B_DBG_TREE;

	BOpType	t = (BOpType)(long)EA_get_data(e);

	switch (t)
	{
		case BOP_NONE:
			t = BOP_NE;
		case BOP_EQ:
		case BOP_GT:
		case BOP_GE:
		case BOP_LT:
		case BOP_LE:
		case BOP_NE:
			break ;
		default:
			todo("handle more conditions in %s | (%s)", __func__,  x_tostr_BOpType(t));
	}
	CG_jump_label(lbl, t);
}

void
CG_function_call(Expression e)
{
	B_DBG_TREE;

	if (e >= arr_count(EA))
		B_compiler_error("invalid Expression.");

	Expressions	arguments = *arr_last(B.arguments);
	Offset		offset = (arr_count(arguments) - 1) * WORD_SIZE;

	CG_stack_reserve(arr_count(arguments) * WORD_SIZE);
	// TODO: push caller saved registers

	arr_foreach_rev(Expression, expr, arguments)
	{
		x86Operand	op_arg = CG_expr_rvalue(*expr);
		x86Operand	slot = MEM_OPERAND(X86_MEM
		(
			.base = X86_REG_ESP,
			.displacement = offset,
		));

		CG_operand_constraints(&slot, &op_arg);
		ASM_mov(slot, op_arg);
		offset -= WORD_SIZE;

		if (op_arg.type == OPERAND_REGISTER)
			RP_register_free(op_arg.reg);
	}

	ExprAlloc	*func = arr_nth(EA, e);

	switch (func->op.type)
	{
		case OPERAND_REGISTER:
		case OPERAND_IMMEDIATE:
		case OPERAND_SYMBOL:
			break ;
		default:
			B_compiler_error("invalid 'call' operand.");
	}
	ASM_call(func->op);

	// TODO: POP caller saved registers

	EA_expr_update(e, REG_OPERAND(X86_REG_EAX), EXPR_STATUS_RESERVED);
}

void
CG_function_arg(Expression e)
{
	B_DBG_TREE;

	// TODO: push expressions inside of B.arguments 
	//   BUT: clean expressions -> just append (evaluation when called)
	//        dirty expressions -> compute in a register, append, generate mov [SLOT], REG when needed.

	if (e >= arr_count(EA))
		B_compiler_error("invalid Expression.");

	ExprAlloc	*alloc = arr_nth(EA, e);
	Expressions	*args = arr_last(B.arguments);

	if (alloc->status != EXPR_STATUS_RESERVED)
	{
		x86Operand	reg = REG_OPERAND(RP_register_alloc(REG_CLASS_NOT_A));

		ASM_mov(reg, alloc->op);

		EA_expr_update(e, reg, EXPR_STATUS_RESERVED);
	}
	arr_append(*args, e);
}

void
CG_char(Expression s, Expression i)
{
	B_DBG_TREE;

	x86Operand	op_s = CG_expr_rvalue(s);
	x86Operand	op_i = CG_expr_rvalue(i);

	if (op_s.type == OPERAND_IMMEDIATE)
		B_compiler_error("invalid operand type for builtin 'char'.");

	if (op_s.type != OPERAND_REGISTER)
	{
		x86Operand	base = REG_OPERAND(RP_register_alloc(REG_CLASS_ANY));
		
		ASM_mov(base, op_s);
		op_s = base;
	}
	if (op_i.type != OPERAND_IMMEDIATE && op_i.type != OPERAND_REGISTER)
	{
		x86Operand	index = REG_OPERAND(RP_register_alloc(REG_CLASS_ANY));
		
		ASM_mov(index, op_i);
		op_i = index;
	}

	x86Operand	result = REG_OPERAND(RP_register_alloc(REG_CLASS_NOT_A));
	x86Operand	byte   = MEM_OPERAND(X86_MEM
	(
		.base         = op_s.reg,
		.index        = op_i.type == OPERAND_REGISTER ? op_i.reg : 0,
		.displacement = op_i.type == OPERAND_IMMEDIATE ? op_i.imm : 0,
		.size         = X86_SIZE_BYTE,
	));

	ASM_movzx(result, byte);
	
	RP_register_free(op_s.reg);
	if (op_i.type == OPERAND_REGISTER)
		RP_register_free(op_i.reg);

	EA_expr_update(s, result, EXPR_STATUS_ALLOCATED);
}

void
CG_lchar(Expression s, Expression i, Expression c)
{
	B_DBG_TREE;

	x86Operand	op_s = CG_expr_rvalue(s);
	x86Operand	op_i = CG_expr_rvalue(i);
	x86Operand	op_c = CG_expr_rvalue(c);

	if (op_s.type == OPERAND_IMMEDIATE)
		B_compiler_error("invalid operand type for builtin 'char'.");

	if (op_s.type != OPERAND_REGISTER)
	{
		x86Operand	base = REG_OPERAND(RP_register_alloc(REG_CLASS_ANY));
		
		ASM_mov(base, op_s);
		op_s = base;
	}
	if (op_i.type != OPERAND_IMMEDIATE && op_i.type != OPERAND_REGISTER)
	{
		x86Operand	index = REG_OPERAND(RP_register_alloc(REG_CLASS_ANY));
		
		ASM_mov(index, op_i);
		op_i = index;
	}

	x86Operand	byte   = MEM_OPERAND(X86_MEM
	(
		.base         = op_s.reg,
		.index        = op_i.type == OPERAND_REGISTER ? op_i.reg : 0,
		.displacement = op_i.type == OPERAND_IMMEDIATE ? op_i.imm : 0,
		.size         = X86_SIZE_BYTE,
	));

	CG_operand_constraints(&byte, &op_c);

	if (op_c.type == OPERAND_REGISTER)
		op_c.reg = X86_REG_MAKE(X86_REG_BASE(op_c.reg), X86_SIZE_BYTE);

	ASM_mov(byte, op_c);
	
	RP_register_free(op_s.reg);
	if (op_i.type == OPERAND_REGISTER)
		RP_register_free(op_i.reg);
	if (op_c.type == OPERAND_REGISTER)
		RP_register_free(op_c.reg);
}
