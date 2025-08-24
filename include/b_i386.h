/**
 * b_i386.h
 */

#include "btypes.h"
#if !defined (_B_I386_H)
# define _B_I386_H

# define    BACKEND             i386
# define    BACKEND_REGS        "esi", "edi", "ebx", "eax", "ecx", "edx"
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
    asm_push(_OP_PREG("ebp"));
    asm_move(_OP_PREG("ebp"), _OP_PREG("esp"));
    asm_stack_reserve(0);
}
 
BACKEND_DEF(epilogue)
{
    Instr   ret;

    asm_move(_OP_PREG("esp"), _OP_PREG("ebp"));
    asm_pop(_OP_PREG("ebp"));
    
    ret = (Instr) { .type = INSTR_RET };
    vec_append(C.code_buffer, ret);
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

BACKEND_DEF(move, Operand dst, Operand src)
{
    Instr   mov = (Instr)
    {
        .type = INSTR_MOVE,
        .dst = dst,
        .op1 = dst,
        .op2 = src,
    };
    vec_append(C.code_buffer, mov);
}

BACKEND_DEF(load, Operand dst, Operand addr) {}
BACKEND_DEF(store, Operand addr, Operand src) {}

BACKEND_DEF(op_bin, BinopType type, Operand dst, Operand a, Operand b)
{
    InstrType   optypes[] = 
    {
    	[BINOP_NULL] = INSTR_NULL,
    	[BINOP_PLUS] = INSTR_ADD,
    	[BINOP_MINUS] = INSTR_SUB,
    	[BINOP_MULT] = INSTR_MUL,
    	[BINOP_DIV] = INSTR_DIV,
    	[BINOP_MOD] = INSTR_MOD,
    	[BINOP_AND] = INSTR_AND,
    	[BINOP_OR] = INSTR_OR,
    	[BINOP_LT] = INSTR_CMP,
    	[BINOP_GT] = INSTR_CMP,
    	[BINOP_LE] = INSTR_CMP,
    	[BINOP_GE] = INSTR_CMP,
    	[BINOP_EQ] = INSTR_CMP,
    	[BINOP_NE] = INSTR_CMP,
    	[BINOP_LSHIFT] = INSTR_SHL,
    	[BINOP_RSHIFT] = INSTR_SHR,
    };

    Instr   op = (Instr)
    {
        .type = optypes[type],
        .dst = dst,
        .op1 = a,
        .op2 = b,
    };
    vec_append(C.code_buffer, op);
}

BACKEND_DEF(op_un, UnopType type, Operand dst, Operand a) {}

BACKEND_DEF(jump, Operand label)
{
    Instr   jmp = (Instr)
    {
        .type = INSTR_JMP,
        .dst = label,
    };
    vec_append(C.code_buffer, jmp);
}

BACKEND_DEF(jump_cc, ConditionType type, Operand label) {}
BACKEND_DEF(call, Operand func) {}
BACKEND_DEF(ret) {}

BACKEND_DEF(push, Operand a)
{
    Instr   push = (Instr)
    {
        .type = INSTR_PUSH,
        .dst = a,
    };
    vec_append(C.code_buffer, push);
}

BACKEND_DEF(pop, Operand a)
{
    Instr   pop = (Instr)
    {
        .type = INSTR_POP,
        .dst = a,
    };
    vec_append(C.code_buffer, pop);
}

BACKEND_DEF(stack_reserve, Size size)
{
    asm_op_bin(BINOP_MINUS, _OP_PREG("esp"), _OP_PREG("esp"), _OP_IMM(size));
}

BACKEND_DEF(stack_release, Size size)
{
    asm_op_bin(BINOP_PLUS, _OP_PREG("esp"), _OP_PREG("esp"), _OP_IMM(size));
}

#endif // _B_I386_H
