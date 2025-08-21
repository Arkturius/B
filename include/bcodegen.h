/**
 * bcodegen.h
 */

#if !defined (_BCODEGEN_H)
# define _BCODEGEN_H

# include <btypes.h>
# include <bcontext.h>

typedef u32                 VReg;

vec_decl(VReg);

typedef struct b_asmbackend ASMBackend;

# define    ASM_FN_POINTER(_name, ...)                                  \
    typedef void    (*FN_##_name)(ASMBackend *, ##__VA_ARGS__)

typedef enum    b_operand_type
{
    OP_VREG,
    OP_MEM,
    OP_IMM,
    OP_LABEL,
}   OperandType;

typedef enum  b_tac_instr_type
{
    INSTR_MOVE,
    INSTR_LOAD,
    INSTR_STORE,
    INSTR_ADD,
    INSTR_SUB,
    INSTR_MUL,
    INSTR_DIV,
    INSTR_MOD,
    INSTR_AND,
    INSTR_OR,
    INSTR_XOR,
    INSTR_SHL,
    INSTR_SHR,
    INSTR_JMP,
    INSTR_JCC,
    INSTR_CALL,
    INSTR_RET,
    INSTR_LABEL,
    INSTR_PUSH,
    INSTR_POP,
    INSTR_ENTER,
    INSTR_LEAVE,
}   InstrType;

typedef enum    b_jump_type
{
    JUMP_E,
    JUMP_L,
    JUMP_B,
    JUMP_G,
    JUMP_A,
    JUMP_C,
    JUMP_NE,
    JUMP_NL,
    JUMP_NB,
    JUMP_NG,
    JUMP_NA,
    JUMP_NC,
}   JumpType;

typedef struct  b_operand
{
    OperandType type;
    union
    {
        VReg    reg;
        Offset  off;
        i64     imm;
        String  lbl;
        void    *ptr;
    };
}   Operand;

typedef struct  b_instr
{
    InstrType   type;
    Operand     dst;
    Operand     op1;
    Operand     op2;
}   Instr;

vec_decl(Instr);

ASM_FN_POINTER(prologue,    String);
ASM_FN_POINTER(epilogue,    String);

ASM_FN_POINTER(push,        Operand);
ASM_FN_POINTER(pop,         Operand);

ASM_FN_POINTER(move,        Operand, Operand);
ASM_FN_POINTER(load,        Operand, Operand);
ASM_FN_POINTER(store,       Operand, Operand);

ASM_FN_POINTER(op_bin,      InstrType, Operand, Operand, Operand);
ASM_FN_POINTER(op_un,       InstrType, Operand, Operand);

ASM_FN_POINTER(jump,        Operand);
ASM_FN_POINTER(jump_cc,     JumpType, Operand);
ASM_FN_POINTER(call,        Operand);
ASM_FN_POINTER(ret);

struct  b_asmbackend
{
    String  name;
    Size    word_size;
    Size    reg_count;

    FN_prologue asm_prologue;
    FN_epilogue asm_epilogue;

    FN_move     asm_move;
    FN_load     asm_load;
    FN_store    asm_store;

    FN_op_bin   asm_op_bin;
    FN_op_un    asm_op_un;

    FN_jump     asm_jump;
    FN_jump_cc  asm_jump_cc;
    FN_call     asm_call;
    FN_ret      asm_ret;

    FN_push     asm_push;
    FN_pop      asm_pop;
};

typedef struct  b_register_pool
{
    VRegs   virtual;
    Strings physical;
    Offset  stack_used;
}   RegisterPool;

typedef struct  b_codebuilder
{
    ASMBackend      back;
    RegisterPool    regs;
    Instrs          code_buffer;
}   CodeBuilder;

extern  CodeBuilder  C;

# if defined (B_NO_PREFIX)
#  define   register_alloc  B_register_alloc
#  define   register_free   B_register_free
# endif

VReg
B_register_alloc(void);

void
B_register_free(VReg reg);


#endif // _BCODEGEN_H
