/**
* codegen.c
*/

#include <stdbool.h>

#include <b.h>
#include <codegen/emission.h>

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

	arr_foreach(ROString, rostr, B.rostrings)
	{
		CG_label(rostr->name, true);
		ASM_dir_string(rostr->content);
	}
	arr_count(B.rostrings) = 0;
}

void
CG_data_scalar_list(StringC name)
{
	B_DBG_TREE;

	CG_switch_section(SECTION_DATA);
	CG_label(name, false);

	ASM_dir_long(NULL, &B.ivals);
	arr_count(B.ivals) = 0;
}

void
CG_data_vector(StringC name, Size size)
{
	B_DBG_TREE;

	CG_switch_section(SECTION_DATA);
	CG_label(name, false);

	if (arr_count(B.ivals) < size)
	{
		arr_realloc(B.ivals, size);
		memset(arr_last(B.ivals) + 1, 0, sizeof(Expression) * (size - arr_count(B.ivals)));
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
		arr_append(B.ivals, EXPR_INVALID);

		CG_label(internal->name, false);
		CG_data_scalar_list(internal->name);
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
	ASM_dir_palign(2);

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

#if defined(B_DEBUG)

void
CG_debug_prolog(void)
{
	B_DBG_TREE;

	ASM_dir_file();
	ASM_dir_loc("");
	ASM_dir_cfi_sp();
}

void
CG_debug_epilog(void)
{
	B_DBG_TREE;

	StringC	func_end = *arr_last(B.labels.stacks[LABEL_FUNC_STOP]);
	StringC	size_dir = B_asprintf("%s, %s - %s", B.function_name, func_end, B.function_name);

	ASM_dir_size(size_dir);
	ASM_dir_cfi_ep();

	free((String) size_dir);
}

#endif // B_DEBUG

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

typedef void	(*FN_asm_setcc)(x86Operand);

void
CG_comparison(Expression e)
{
	B_DBG_TREE;

	BOpType	type = (BOpType)(long)EA_get_data(e);

	static const FN_asm_setcc	setcc_functions[BOP_XENUM_LAST] = 
	{
		[BOP_EQ  ] = ASM_setne,
		[BOP_NE  ] = ASM_sete,
		[BOP_LT  ] = ASM_setge,
		[BOP_LE  ] = ASM_setg,
		[BOP_GT  ] = ASM_setle,
		[BOP_GE  ] = ASM_setl,
	};
	switch (type)
	{
		case BOP_NONE:
			type = BOP_NE;
		case BOP_EQ:
		case BOP_NE:
		case BOP_GT:
		case BOP_GE:
		case BOP_LT:
		case BOP_LE:
			break ;
		default:
			unreachable("invalid BOpType for 'setcc'.");
	}

	x86Register	reg = RP_register_alloc(REG_CLASS_ANY);
	x86Operand	word = REG_OPERAND(reg);
	x86Operand	byte = REG_OPERAND(X86_REG_MAKE(X86_REG_BASE(reg), X86_SIZE_BYTE));

	setcc_functions[type](byte);
	ASM_movzx(word, byte);

	EA_expr_update(e, word, EXPR_STATUS_RESERVED);
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

BOpType
TEST_SWITCH_COMPARE(BOpType type)
{
	switch (type)
	{
		case BOP_NONE: return BOP_NE;
		case BOP_EQ: return BOP_NE;
		case BOP_NE: return BOP_EQ;
		case BOP_LT: return BOP_GE;
		case BOP_GT: return BOP_LE;
		case BOP_LE: return BOP_GT;
		case BOP_GE: return BOP_LT;
		default:
			unreachable("invalid BOpType for switching comparison.");
	}
}

x86Operand
CG_expr_rvalue(Expression e)
{
	B_DBG_TREE;

	if (e >= arr_count(EA))
		B_compiler_error("out of bounds Expression.");

	ExprAlloc	*ex = arr_nth(EA, e);
	Symbol		*sym = (Symbol *)EA_get_data(e);

	if (ex->status == EXPR_STATUS_COMPARE)
	{
		ex->status = EXPR_STATUS_RESERVED;
		CG_expr_condition(e);

		ex->data = (void *)TEST_SWITCH_COMPARE((BOpType)(long)ex->data);
		CG_comparison(e);
	}
	else if (B_is_symbol(sym) && B_symbol_is_variable(sym, VARIABLE_VECTOR))
	{
		x86Operand	reg = REG_OPERAND(RP_register_alloc(REG_CLASS_ANY));

		switch (ex->op.type)
		{
			case OPERAND_MEMORY:
			case OPERAND_SYMBOL:
				ASM_lea(reg, ex->op);
				break ;
			default:
				unreachable("%s: invalid OperandType for VECTOR.", __func__);
		}
		EA_expr_update(e, reg, EXPR_STATUS_RESERVED);
	}
	return (ex->op);
}

void
CG_expr_condition(Expression e)
{
	B_DBG_TREE;

	ExprAlloc	*alloc = arr_nth(EA, e);
	x86Operand	op_e = CG_expr_rvalue(e);

	Expression	b = (Expression)(long)EA_get_data(e);

	if (b > 0)
	{
		x86Operand	op_b = CG_expr_rvalue(b);

		CG_operand_constraints(&op_e, &op_b);
		if (op_e.type == OPERAND_MEMORY)
			op_e.mem.size = X86_SIZE_DWORD;

		ASM_cmp(op_e, op_b);

		if (op_e.type == OPERAND_REGISTER)
			RP_register_free(op_e.reg);
		if (op_b.type == OPERAND_REGISTER)
			RP_register_free(op_b.reg);
		alloc->data = EA_get_data(b);
	}
	else
	{
		CG_test(e);
		alloc->data = (void *)BOP_NE;	
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

void
CG_ternary(Expression pred, Expression yes, Expression no)
{
	B_DBG_TREE;

	x86Operand	op_yes = CG_expr_rvalue(yes);
	x86Operand	op_no = CG_expr_rvalue(no);

	if (op_yes.type != OPERAND_REGISTER)
	{
		x86Operand	reg_yes = REG_OPERAND(RP_register_alloc(REG_CLASS_ANY));

		ASM_mov(reg_yes, op_yes);
		op_yes = reg_yes;
	}
	if (op_no.type != OPERAND_REGISTER)
	{
		x86Operand	reg_no = REG_OPERAND(RP_register_alloc(REG_CLASS_ANY));

		ASM_mov(reg_no, op_no);
		op_no = reg_no;
	}

	CG_test(pred);
	ASM_cmove(op_yes, op_no);

	RP_register_free(op_no.reg);

	EA_expr_update(pred, op_yes, EXPR_STATUS_RESERVED);
}

typedef void	(*FN_asm_binop)(x86Operand, x86Operand);

void
CG_binop(BOpType op, Expression a, Expression b, bool in_place)
{
	B_DBG_TREE;

	static const FN_asm_binop	binop_functions[BOP_XENUM_LAST] = 
	{
		[BOP_PLUS ] = ASM_add,
		[BOP_MINUS] = ASM_sub,
		[BOP_AND  ] = ASM_and,
	};
	// get rid of switch case when all binop are implemented
	switch (op)
	{
		case BOP_MINUS:
		case BOP_PLUS:
		case BOP_AND:
			break ;
		default:
			todo("%s: implement func pointer for (%s)", __func__, x_tostr_BOpType(op));
	}

	x86Operand	op_a = CG_expr_rvalue(a);
	x86Operand	op_b = CG_expr_rvalue(b);
	x86Operand	acc  = {0};

	if (op_a.type == OPERAND_MEMORY)
		op_a.mem.size = X86_SIZE_DWORD;

	in_place = in_place && (op_a.type == OPERAND_MEMORY || op_a.type == OPERAND_SYMBOL);
	in_place = in_place && (op_b.type == OPERAND_IMMEDIATE || op_b.type == OPERAND_REGISTER);
	
	if (!in_place)
	{
		acc = REG_OPERAND(RP_register_alloc(REG_CLASS_ACCUM));
		ASM_mov(acc, op_a);
	}
	else
		acc = op_a;
	binop_functions[op](acc, op_b);
	EA_expr_update(a, acc, EXPR_STATUS_ALLOCATED);

	if (!in_place && op_a.type == OPERAND_REGISTER)
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

	static const FN_asm_jump	jump_functions[BOP_XENUM_LAST] = 
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
			unreachable("invalid BOpType for 'jcc'");
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
CG_subscript(Expression arr, Expression idx)
{
	B_DBG_TREE;

	x86Operand	op_arr = CG_expr_rvalue(arr);
	x86Operand	op_idx = CG_expr_rvalue(idx);
	x86Memory	sub = { .scale = X86_MEM_SCALE_DWORD, .size = X86_SIZE_ARCH };

	switch (op_arr.type)
	{
		case OPERAND_MEMORY:
		{
			if (op_arr.type == OPERAND_MEMORY && op_arr.mem.base == X86_REG_EBP)
				sub = op_arr.mem;
			else 
			{
				if (op_arr.type == OPERAND_MEMORY)
				{
					x86Register	reg = RP_register_alloc(REG_CLASS_NOT_A);
					
					ASM_lea(REG_OPERAND(reg), op_arr);
					EA_expr_update(arr, REG_OPERAND(reg), EXPR_STATUS_RESERVED);
					sub = X86_MEM(.base = reg);
				}
			}
			break ;
		}
		case OPERAND_SYMBOL:
		{
			x86Register	reg = RP_register_alloc(REG_CLASS_ANY);

			ASM_mov(REG_OPERAND(reg), op_arr);
			op_arr = REG_OPERAND(reg);
		}
		__attribute__((fallthrough));
		case OPERAND_REGISTER:
			sub = X86_MEM(.base = op_arr.reg);
		__attribute__((fallthrough));
		case OPERAND_IMMEDIATE:
			sub.displacement = op_arr.imm;
			break ;
		default:
			unreachable("invalid OperandType for '[]' operator.");
	}
	switch (op_idx.type)
	{
		case OPERAND_IMMEDIATE:
			sub.displacement += WORD_SIZE * op_idx.imm;
			break ;
		case OPERAND_MEMORY:
		{
			x86Register	reg = RP_register_alloc(REG_CLASS_ANY);

			ASM_mov(REG_OPERAND(reg), op_idx);
			EA_expr_update(idx, REG_OPERAND(reg), EXPR_STATUS_RESERVED);
			op_idx = REG_OPERAND(reg);
		}
		__attribute__((fallthrough));
		case OPERAND_REGISTER:
			sub.index = op_idx.reg;
			break ;
		default:
			unreachable("invalid OperandType for '[]' operator.");
	}

	EA_expr_update(arr, MEM_OPERAND(sub), EXPR_STATUS_RESERVED);

	if (op_idx.type == OPERAND_REGISTER)
		RP_register_free(op_idx.reg);
}

void
CG_deref(Expression e)
{
	B_DBG_TREE;

	x86Operand	op_e = CG_expr_rvalue(e);
	x86Operand	reg = {0};
	x86Operand	deref = {0};

	switch (op_e.type)
	{
		case OPERAND_MEMORY:
		{
			reg   = REG_OPERAND(RP_register_alloc(REG_CLASS_ANY));
			deref = op_e;
			break ;
		}
		case OPERAND_REGISTER:
		{
			reg   = REG_OPERAND(op_e.reg);
			deref = MEM_OPERAND(X86_MEM(.base = op_e.reg));
			break ;
		}
		case OPERAND_SYMBOL:
		{
			reg   = REG_OPERAND(RP_register_alloc(REG_CLASS_ANY));
			deref = SYM_OPERAND(op_e.sym);
			break ;
		}
		default:
			unreachable("invalid operand type for deref");
	}
	ASM_mov(reg, deref);
	reg = MEM_OPERAND(X86_MEM(.base = reg.reg));
	EA_expr_update(e, reg, EXPR_STATUS_RESERVED);
}

void
CG_invert(Expression e)
{
	x86Operand	op_e = CG_expr_rvalue(e);

	switch (op_e.type)
	{
		case OPERAND_IMMEDIATE:
		{
			EA_expr_update(e, IMM_OPERAND(!op_e.imm), EXPR_STATUS_RESERVED);
			return ;
		}
		case OPERAND_MEMORY:
		case OPERAND_REGISTER:
			break ;
		default:
			unreachable("implement invert for operand (%s)", x_tostr_x86OperandType(op_e.type));
	}
	CG_test(e);
	CG_comparison(e);
}

void
CG_negate(Expression e)
{
	x86Operand	op_e = CG_expr_rvalue(e);

	switch (op_e.type)
	{
		case OPERAND_IMMEDIATE:
		{
			EA_expr_update(e, IMM_OPERAND(!op_e.imm), EXPR_STATUS_RESERVED);
			return ;
		}
		default:
			unreachable("implement negation for operand (%s)", x_tostr_x86OperandType(op_e.type));
	}
	ASM_neg(op_e);
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
CG_incr(Expression e)
{
	B_DBG_TREE;

	x86Operand	op_e = CG_expr_lvalue(e);

	ASM_add(op_e, IMM_OPERAND(1));
}

void
CG_decr(Expression e)
{
	B_DBG_TREE;

	x86Operand	op_e = CG_expr_lvalue(e);

	ASM_sub(op_e, IMM_OPERAND(1));
}

void
CG_post_incr(Expression e)
{
	B_DBG_TREE;

	x86Operand	op_e = CG_expr_lvalue(e);
	x86Operand	reg = REG_OPERAND(RP_register_alloc(REG_CLASS_ANY));

	ASM_mov(reg, op_e);
	CG_incr(e);
	EA_expr_update(e, reg, EXPR_STATUS_RESERVED);
}

void
CG_post_decr(Expression e)
{
	B_DBG_TREE;

	x86Operand	op_e = CG_expr_lvalue(e);
	x86Operand	reg = REG_OPERAND(RP_register_alloc(REG_CLASS_ANY));

	ASM_mov(reg, op_e);
	CG_decr(e);
	EA_expr_update(e, reg, EXPR_STATUS_RESERVED);
}

void
CG_addrof(Expression e)
{
	B_DBG_TREE;
	
	x86Operand	op_e = CG_expr_rvalue(e);

	switch (op_e.type)
	{
		case OPERAND_MEMORY:
		{
			if (op_e.mem.base == X86_REG_NUL)
			{
				if (op_e.mem.index == X86_REG_NUL)
				{
					x86Operand	imm = IMM_OPERAND(op_e.mem.displacement);

					EA_expr_update(e, imm, EXPR_STATUS_RESERVED);
					break ;
				}
			}
		}
		__attribute__((fallthrough));
		case OPERAND_SYMBOL:
		{
			x86Operand	reg = REG_OPERAND(RP_register_alloc(REG_CLASS_ACCUM));

			ASM_lea(reg, op_e);
			EA_expr_update(e, reg, EXPR_STATUS_RESERVED);
			break ;
		}
		default:
			unreachable("invalid x86Operand for '&' operator.");
	}
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
