/**
 * bcodegen.h
 */

#if !defined (_BCODEGEN_H)
# define _BCODEGEN_H

# include <btypes.h>
# include <bcontext.h>

typedef u32                 VReg;

vec_decl(VReg);

typedef struct c_asmbackend     ASMBackend;
typedef struct c_register_pool  RegisterPool;

typedef enum    c_operand_type
{
    OP_NULL,
    OP_VREG,
    OP_PREG,
    OP_MEM,
    OP_IMM,
    OP_LABEL,
}   OperandType;

typedef enum  c_instr_type
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
    INSTR_DIRECTIVE,
}   InstrType;

typedef enum    c_jump_type
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

typedef enum    c_directive_type
{
    DIR_SYNTAX,
    DIR_SECTION,
    DIR_GLOBAL,
    DIR_EXTERN,
    DIR_ALIGN,
    DIR_STRING,
    DIR_BYTE,
    DIR_WORD,
    DIR_DWORD,
    DIR_QWORD,
    DIR_LABEL,
}   DirectiveType;

typedef struct  c_operand
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
    Size    size;
}   Operand;

# define _OP_VREG(_n)   (Operand){.type = OP_VREG, .reg = _n}
# define _OP_PREG(_s)   (Operand){.type = OP_PREG, .lbl = _s}
# define _OP_MEM(_o)    (Operand){.type = OP_MEM, .off = _o}
# define _OP_IMM(_i)    (Operand){.type = OP_IMM, .imm = _i}
# define _OP_LABEL(_l)  (Operand){.type = OP_LABEL, .lbl = _l}

typedef struct  c_instr
{
    InstrType   type;
    Operand     dst;
    Operand     op1;
    Operand     op2;
}   Instr;

vec_decl(Instr);

# define    BACK_FN_LIST(X)                                             \
    X(prologue)                                                         \
    X(epilogue)                                                         \
    X(move,         Operand dst, Operand src)                           \
    X(load,         Operand dst, Operand addr)                          \
    X(store,        Operand addr, Operand src)                          \
    X(op_bin,       InstrType type, Operand dst, Operand a, Operand b)  \
    X(op_un,        InstrType type, Operand dst, Operand a)             \
    X(jump,         Operand label)                                      \
    X(jump_cc,      JumpType type, Operand label)                       \
    X(call,         Operand func)                                       \
    X(ret)                                                              \
    X(push,         Operand a)                                          \
    X(pop,          Operand a)                                          \
    X(directive,    DirectiveType type, Operand data)                   \

# define    ASM_FN_POINTER(_name, ...)                                  \
    typedef void    (*CONCAT(FN_, _name))(ASMBackend *, ##__VA_ARGS__)

# define    DECLARE_FN_VARS(_name, ...)                                 \
    CONCAT(FN_, _name) CONCAT(asm_, _name);

# define    DECLARE_FN_TYPES(_name, ...)                                \
    ASM_FN_POINTER(_name, ##__VA_ARGS__);
    
# define    DECLARE_FN_NAMES(_name, ...)                                \
    STRINGIFY(CONCAT(asm_, _name)),

BACK_FN_LIST(DECLARE_FN_TYPES)

typedef bool    (*FN_dump)(ASMBackend *);

struct  c_asmbackend
{
    String  name;
    Size    word_size;
    Size    reg_count;
    
    BACK_FN_LIST(DECLARE_FN_VARS);

    FN_dump asm_dump;
};

struct  c_register_pool
{
    VRegs   virtual;
    Strings physical;
    Offset  stack_used;
};

bool
C_registers_set(Size count, ...);

typedef bool    (*FN_backend)(ASMBackend *);

bool
C_builder_start(FN_backend backend_setup);

void
C_builder_stop(void);

typedef struct  c_codebuilder
{
    ASMBackend      back;
    RegisterPool    regs;
    Instrs          code_buffer;
}   CodeBuilder;

extern  CodeBuilder  C;

# define	asm_prologue(...)   	C.back.asm_prologue(&C.back, ##__VA_ARGS__)
# define	asm_epilogue(...)   	C.back.asm_epilogue(&C.back, ##__VA_ARGS__)
# define	asm_move(...)   		C.back.asm_move(&C.back, ##__VA_ARGS__)
# define	asm_load(...)   		C.back.asm_load(&C.back, ##__VA_ARGS__)
# define	asm_store(...)   		C.back.asm_store(&C.back, ##__VA_ARGS__)
# define	asm_op_bin(...)   		C.back.asm_op_bin(&C.back, ##__VA_ARGS__)
# define	asm_op_un(...)   		C.back.asm_op_un(&C.back, ##__VA_ARGS__)
# define	asm_jump(...)   		C.back.asm_jump(&C.back, ##__VA_ARGS__)
# define	asm_jump_cc(...)   		C.back.asm_jump_cc(&C.back, ##__VA_ARGS__)
# define	asm_call(...)   		C.back.asm_call(&C.back, ##__VA_ARGS__)
# define	asm_ret(...)   			C.back.asm_ret(&C.back, ##__VA_ARGS__)
# define	asm_push(...)   		C.back.asm_push(&C.back, ##__VA_ARGS__)
# define	asm_pop(...)   			C.back.asm_pop(&C.back, ##__VA_ARGS__)
# define	asm_directive(...)   	C.back.asm_directive(&C.back, ##__VA_ARGS__)
# define	asm_dump(...)           C.back.asm_dump(&C.back, ##__VA_ARGS__)

#endif // _BCODEGEN_H

# if defined (BACKEND_DECLARATION)
#  if !defined (BACKEND)
#   error "BACKEND not defined"
#  endif
#  if !defined (BACKEND_REGS)
#   error "BACKEND_REGS not defined"
#  endif
#  if !defined (BACKEND_REG_MAX)
#   error "BACKEND_REG_MAX not defined."
#  endif
#  if !defined (BACKEND_WORD_SIZE)
#   error "BACKEND_WORD_SIZE not defined"
#  endif

#   define  BACKEND_NAME        STRINGIFY(BACKEND)
#   define  BACKEND_FUNC(_n)    CONCAT(CONCAT(BACKEND, _), _n)

#   define  BACKEND_DECL(_n, ...)                                       \
        void BACKEND_FUNC(_n)(ASMBackend *back, ##__VA_ARGS__);

#   define  BACKEND_DEF(_n, ...)                                        \
        void BACKEND_FUNC(_n)(ASMBackend *back, ##__VA_ARGS__)

#   define  BACKEND_EMPTY_DEF(_n, ...)                                  \
        void BACKEND_FUNC(_n)(ASMBackend *back, ##__VA_ARGS__) {}

# define    BACK_FN_BIND(_name, ...)                                    \
    CONCAT(BACKEND, _back)->CONCAT(asm_, _name) = BACKEND_FUNC(_name);

BACK_FN_LIST(BACKEND_DECL)

bool
BACKEND_FUNC(dump)(ASMBackend *back);

bool
CONCAT(BACKEND, _setup)(ASMBackend *CONCAT(BACKEND, _back))
{
    if (!CONCAT(BACKEND, _back))
        return (false);
    CONCAT(BACKEND, _back)->name = BACKEND_NAME;
    CONCAT(BACKEND, _back)->reg_count = BACKEND_REG_MAX;
    CONCAT(BACKEND, _back)->word_size = BACKEND_WORD_SIZE;
    BACK_FN_LIST(BACK_FN_BIND)
    CONCAT(BACKEND, _back)->asm_dump = BACKEND_FUNC(dump);
    return (C_registers_set(BACKEND_REG_MAX, BACKEND_REGS));
}

#endif // BACKEND_DECLARATION
