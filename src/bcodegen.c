/**
 * bcodegen.c
 */

#include <string.h>
#include <stdarg.h>

#define B_NO_PREFIX
#include <btypes.h>
#include <bcontext.h>
#include <bcodegen.h>
#include <stdbool.h>

CodeBuilder C = {0};

bool
C_registers_set(Size count, ...)
{
    va_list ap;

    if (count == 0)
    {
        B_error("%s: no physical registers.", __func__);
        return (false);
    }

    va_start(ap, count);
    while (count)
    {
        String  reg = va_arg(ap, String);

        if (!reg)
        {
            B_error("%s: NULL register.", __func__);
            return (false);
        }
        vec_append(C.regs.physical, reg);
        count--;
    }
    va_end(ap);
    return (true);
}

bool
C_builder_start(FN_backend backend_setup)
{
    if (!backend_setup)
    {
        B_error("%s: no backend setup provided.", __func__);
        return (false);
    }
    return (backend_setup(&C.back));
}

Expr
C_register_alloc()
{
    Expr            reg;
    RegisterPool    *pool = &C.regs;

    if (pool->physical.count > 0)
    {
        u32     idx = pool->physical.count - 1;
        String  next = pool->physical.items[idx];

        reg = (Expr) { .type = EXPR_PREGISTER, .reg = next};
        vec_append(pool->used_physical, next);
        vec_pop(pool->physical, 1);

        B_log("Register Allocation -> '%s'", next);
    }
    else
    {
        reg = (Expr) { .type = EXPR_VREGISTER, .imm = pool->next_virtual};
        vec_append(pool->used_virtual, pool->next_virtual);
        B_log("Register Allocation -> 'v%d'", pool->next_virtual);
        pool->next_virtual++;
    }
    return (reg);
}

void
C_register_free(Operand reg)
{
    RegisterPool    *pool = &C.regs;

    if (reg.type == OP_PREG)
    {
        vec_foreach(String, preg, pool->used_physical)
        {
            if (!strcmp(*preg, reg.lbl))
            {
                vec_append(pool->physical, *preg);
                vec_delete(pool->used_physical, vec_index(pool->used_physical, preg));
                B_log("Register Release    -> '%s'", *preg);
                return ;
            }
        }
    }
    else if (reg.type == OP_VREG)
    {
        vec_foreach(VReg, vreg, pool->used_virtual)
        {
            if (*vreg == reg.reg)
            {
                vec_delete(pool->used_virtual, vec_index(pool->used_virtual, vreg));
                B_log("Register Release    -> 'v%d'", pool->next_virtual);
                return ;
            }
        }
    }
    else
    {
        B_error("unexpected operand '%s'", op_type(reg.type));
        compiler_stop();
        exit(1);
    }
}

/** 
 * DUMP FUNCTIONS
 */

String
op_type(OperandType t)
{
    switch (t)
    {
        case OP_NULL: return "OP_NULL";
		case OP_VREG: return "OP_VREG";
		case OP_PREG: return "OP_PREG";
		case OP_MEM: return "OP_MEM";
		case OP_IMM: return "OP_IMM";
		case OP_LABEL: return "OP_LABEL";
        default: return "UNKNOWN";
    }
}

String
instr_type(InstrType t)
{
    switch (t) 
    {
		case INSTR_MOVE: return "INSTR_MOVE";
		case INSTR_LOAD: return "INSTR_LOAD";
		case INSTR_STORE: return "INSTR_STORE";
		case INSTR_ADD: return "INSTR_ADD";
		case INSTR_SUB: return "INSTR_SUB";
		case INSTR_MUL: return "INSTR_MUL";
		case INSTR_DIV: return "INSTR_DIV";
		case INSTR_MOD: return "INSTR_MOD";
		case INSTR_AND: return "INSTR_AND";
		case INSTR_OR: return "INSTR_OR";
		case INSTR_XOR: return "INSTR_XOR";
		case INSTR_SHL: return "INSTR_SHL";
		case INSTR_SHR: return "INSTR_SHR";
		case INSTR_JMP: return "INSTR_JMP";
		case INSTR_JCC: return "INSTR_JCC";
		case INSTR_CALL: return "INSTR_CALL";
		case INSTR_RET: return "INSTR_RET";
		case INSTR_LABEL: return "INSTR_LABEL";
		case INSTR_PUSH: return "INSTR_PUSH";
		case INSTR_POP: return "INSTR_POP";
		case INSTR_DIRECTIVE: return "INSTR_DIRECTIVE";
        default: return "UNKNOWN";
    }
}

String
directive_type(DirectiveType t)
{
    switch (t)
    {
		case DIR_SYNTAX: return "DIR_SYNTAX";
		case DIR_SECTION: return "DIR_SECTION";
		case DIR_GLOBAL: return "DIR_GLOBAL";
		case DIR_EXTERN: return "DIR_EXTERN";
		case DIR_ALIGN: return "DIR_ALIGN";
		case DIR_STRING: return "DIR_STRING";
		case DIR_BYTE: return "DIR_BYTE";
		case DIR_WORD: return "DIR_WORD";
		case DIR_DWORD: return "DIR_DWORD";
		case DIR_QWORD: return "DIR_QWORD";
		case DIR_LABEL: return "DIR_LABEL";
        default: return "UNKNOWN";
    }
}

void
dump_operand(Operand op, bool is_directive)
{
    if (op.type == OP_NULL)
        return ;
    printf("  \033[90;1m%-8s\033[0m, ", op_type(op.type));
    switch (op.type)
    {
        case OP_VREG:
            printf("reg = \033[31mv%d\033[0m", op.reg); break ;
        case OP_PREG:
            printf("reg = \033[31m%s\033[0m", op.lbl); break ;
        case OP_MEM:
            printf("mem = \033[34m%d\033[0m", op.off); break ;
        case OP_IMM:
            if (!is_directive)
                printf("imm = \033[32m%ld\033[0m", op.imm);
            else
                printf("dir = %s", directive_type(op.imm));
            break ;
        case OP_LABEL:
            printf("lbl = \033[1m'%s'\033[0m", op.lbl); break ;
        default:
            break ;
    }
    printf("\n");
}

void
dump_instr(Instr *ins)
{
    printf("\033[36;1m%s\033[0m:\n", instr_type(ins->type));
    if (ins->dst.type != OP_NULL)
        dump_operand(ins->dst, ins->type == INSTR_DIRECTIVE);
    if (ins->op1.type != OP_NULL)
        dump_operand(ins->op1, ins->type == INSTR_DIRECTIVE);
    if (ins->op2.type != OP_NULL)
        dump_operand(ins->op2, ins->type == INSTR_DIRECTIVE);
    printf("\n");
}
