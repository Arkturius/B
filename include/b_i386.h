/**
 * b_i386.h
 */

#include "bparser.h"
#include "btypes.h"
#if !defined (_B_I386_H)
# define _B_I386_H

# define    BACKEND             i386
# define    BACKEND_REGS        "edx", "ecx", "eax", "ebx", "edi", "esi"
# define    BACKEND_REG_MAX     6
# define    BACKEND_WORD_SIZE   4
# define    BACKEND_DECLARATION
# include   <bcodegen.h>

bool
i386_dump_directive(Instr *directive)
{
    DirectiveType   type;
    String          prefix;
    String          data;

    type = directive->dst.imm;
    data = type != DIR_SYNTAX ? directive->op1.lbl : "noprefix";
    switch (type)
    {
        case DIR_LABEL:
            printf("%s:\n", data);
            return (true);
        case DIR_SYNTAX:  prefix = ".intel_syntax"; break ;
        case DIR_SECTION: prefix = ".section";      break ;
        case DIR_GLOBAL:  prefix = ".globl";        break ;
        case DIR_ALIGN:   prefix = ".align";        break ;
        case DIR_STRING:  prefix = ".string";       break ;
        case DIR_BYTE:    prefix = ".byte";         break ;
        case DIR_WORD:    prefix = ".short";        break ;
        case DIR_DWORD:   prefix = ".long";         break ;
        default:
            B_error("unsupported assembler directive.");
            return (false);
            break ;
    }
    printf("%s %s\n", prefix, data);
    return (true);
}

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

bool 
i386_dump(ASMBackend *back)
{
    bool    ret = true;

    vec_foreach(Instr, instr, C.code_buffer)
    {
        dump_instr(instr);
        /*
        switch (instr->type)
        {
            case INSTR_MOVE:
            case INSTR_LOAD:
            case INSTR_STORE:
            case INSTR_ADD:
            case INSTR_SUB:
            case INSTR_MUL:
            case INSTR_DIV:
            case INSTR_MOD:
            case INSTR_AND:
            case INSTR_OR:
            case INSTR_XOR:
            case INSTR_SHL:
            case INSTR_SHR:
            case INSTR_JMP:
            case INSTR_JCC:
            case INSTR_CALL:
            case INSTR_RET:
            case INSTR_LABEL:
            case INSTR_PUSH:
            case INSTR_POP:
            default:
                break ;
            case INSTR_DIRECTIVE:
                ret = i386_dump_directive(instr);
        }
        if (!ret)
            break ;
        */
    }
    vec_count(C.code_buffer) = 0;
    return (ret);
}

BACKEND_DEF(prologue)
{
    Instr   tmp;

    tmp = (Instr)
    {
        .type = INSTR_PUSH,
        .op1 = _OP_PREG("ebp"),
    };
    vec_append(C.code_buffer, tmp);
    
    tmp = (Instr)
    {
        .type = INSTR_MOVE,
        .dst = _OP_PREG("ebp"),
        .op1 = _OP_PREG("ebp"),
        .op2 = _OP_PREG("esp"),
    };
    vec_append(C.code_buffer, tmp);

    tmp = (Instr)
    {
        .type = INSTR_SUB,
        .dst = _OP_PREG("esp"),
        .op1 = _OP_PREG("esp"),
        .op2 = _OP_IMM(0),
    };
    vec_append(C.code_buffer, tmp);
}
 
BACKEND_DEF(epilogue)
{
    Instr   tmp;

    tmp = (Instr)
    {
        .type = INSTR_MOVE,
        .dst = _OP_PREG("esp"),
        .op1 = _OP_PREG("esp"),
        .op2 = _OP_PREG("ebp"),
    };
    vec_append(C.code_buffer, tmp);
    
    tmp = (Instr)
    {
        .type = INSTR_POP,
        .op1 = _OP_PREG("ebp"),
    };
    vec_append(C.code_buffer, tmp);
    
    tmp = (Instr)
    {
        .type = INSTR_RET,
    };
    vec_append(C.code_buffer, tmp);
}

BACKEND_DEF(directive, DirectiveType type, Operand data)
{
    Instr   dir = (Instr)
    {
        .type = INSTR_DIRECTIVE,
        .dst = _OP_IMM(type), 
        .op1 = data,
    };
    vec_append(C.code_buffer, dir);
}

BACKEND_DEF(move, Operand dst, Operand src) {}
BACKEND_DEF(load, Operand dst, Operand addr) {}
BACKEND_DEF(store, Operand addr, Operand src) {}
BACKEND_DEF(op_bin, InstrType type, Operand dst, Operand a, Operand b) {}
BACKEND_DEF(op_un, InstrType type, Operand dst, Operand a) {}
BACKEND_DEF(jump, Operand label) {}
BACKEND_DEF(jump_cc, JumpType type, Operand label) {}
BACKEND_DEF(call, Operand func) {}
BACKEND_DEF(ret) {}
BACKEND_DEF(push, Operand a) {}
BACKEND_DEF(pop, Operand a) {}

#endif // _B_I386_H
