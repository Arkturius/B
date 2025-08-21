/**
 * codegen.h
 */

#ifndef _CODEGEN_H
# define _CODEGEN_H

# include <stdio.h>
# include <string.h>
# include <stdarg.h>

# include <btypes.h>
# include <bloc.h>

# define	EAX	"eax"
# define	EBX	"ebx"
# define	ECX	"ecx"
# define	EDX	"edx"
# define	ESP	"esp"
# define	EBP	"ebp"
# define	EDI	"edi"
# define	ESI	"esi"

String  x86REGS[5] = { EAX, ECX, EDX, ESI, EDI };

# define    REG_MAX         64
# define	EMIT(X, ...)	printf(X"\n", ##__VA_ARGS__)

char    cgtmp[1024] = {0};

String
CG_sprintf(const char *fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	vsnprintf(cgtmp, sizeof(cgtmp), fmt, ap);
	va_end(ap);

	return (cgtmp);
}

String
CG_asprintf(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(cgtmp, sizeof(cgtmp), fmt, ap);
    va_end(ap);
    cgtmp[sizeof(cgtmp) - 1] = 0;

    return (strdup(cgtmp));
}

typedef enum    _cg_instr_type
{
    CG_INSTR_PUSH,
    CG_INSTR_POP,

    CG_INSTR_MOV,
    CG_INSTR_MOVZX,

    CG_INSTR_ADD,
    CG_INSTR_SUB,

    CG_INSTR_TEST,
    CG_INSTR_CMP,

    CG_INSTR_JMP,
    CG_INSTR_JCC,
    CG_INSTR_CALL,
    CG_INSTR_RET,

    CG_INSTR_DIRECTIVE,
}   InstrType;

typedef enum _cg_operand_type
{
    CG_OP_NUL,
    CG_OP_REG,
    CG_OP_MEM,
    CG_OP_IMM,
    CG_OP_LBL,
}   OperandType;

typedef struct _cg_operand
{
    OperandType type;
    union
    {
        struct
        {
            union
            {
                String  base;
                String  label;
            };
            String  index;
            u64     scale;
            i64     displacement;
        };
        i64 immediate;
    };
    u32 size;
}   Operand;

typedef struct  _cg_instr
{
    InstrType   type;
    Operand     op1;
    Operand     op2;
}   Instr;

BLOC_DECL(Instr);

Instrs    chain = {0};

static inline void
_CG_instr(Instr i)
{
    BLOC_APPEND(chain, i);
}
# define    CG_instr(t, ...)    _CG_instr((Instr){.type = t, ##__VA_ARGS__ })

# define    OP_REG(_r)          (Operand){.type = CG_OP_REG, .base = _r}
# define    OP_MEM(...)         (Operand){.type = CG_OP_MEM, .size = 2, __VA_ARGS__ }
# define    OP_IMM(_i)          (Operand){.type = CG_OP_IMM, .immediate = _i}
# define    OP_LBL(_l)          (Operand){.type = CG_OP_LBL, .label = _l}

# define    CG_MEM_SCALE(x)     (!x || x == 1 || x == 2 || x == 4 || x == 8)

typedef String  Register;

BLOC_DECL(Register);

Registers   regs = {0};

String
CG_register_alloc()
{
    static bool init = false;

    String  name;
    i32     idx;

    if (!init)
    {
        for (u32 i = 0; i < 5; ++i)
            BLOC_APPEND(regs, NULL);
        init = true; 
    }
    idx = -1;
    BLOC_FOREACH(Register, reg, regs)
    {
        ++idx;
        if (*reg)
            continue ;
        break ;
    }
    if (idx == -1)
        ++idx;
    if (idx < 5)
        name = strdup(x86REGS[idx]);
    else
        name = CG_asprintf("t%d", idx - 5);

    if (idx >= (i32) BLOC_COUNT(regs))
        BLOC_APPEND(regs, name);
    else
        regs.bloc[idx] = name;

    return (name);
}

void
CG_register_free(String name)
{
    BLOC_FOREACH(Register, reg, regs)
    {
        if (!*reg)
            continue ;
        if (!strcmp(*reg, name))
        {
            *reg = NULL;
            return ;
        }
    }
    abort();
}

String
CG_operand(Operand *op, bool destination)
{
    String  sizes[3] = {"BYTE", "WORD", "DWORD"};
    bool    started = false;
    char    buf[64] = {0};

    switch (op->type)
    {
        case CG_OP_IMM:
            strcat(buf, CG_sprintf("0x%02lx", (i64) op->immediate));
            break ;
        case CG_OP_REG:
            strcat(buf, op->base);
            break ;
        case CG_OP_MEM:
        {
            if (op->size > 2)
                break ;
            if (destination)
                strcat(buf, CG_sprintf("%s PTR ", sizes[op->size]));
            strcat(buf, "[");
            if (op->base)
            {
                strcat(buf, CG_sprintf("%s", op->base));
                started = true;
            }
            if (op->index && CG_MEM_SCALE(op->scale))
            {
                if (started)
                    strcat(buf, " + ");
                strcat(buf, CG_sprintf("%s", op->index));
                if (op->scale > 1)
                    strcat(buf, CG_sprintf(" * 0x%02x", op->index, op->scale));
            }
            if (op->displacement != 0)
            {
                if (started)
                {
                    if (op->displacement > 0)
                        strcat(buf, CG_sprintf(" + 0x%02x", op->displacement));
                    else
                        strcat(buf, CG_sprintf(" - 0x%02x", -op->displacement));
                }
                else
                    strcat(buf, CG_sprintf("%d", op->displacement));
            }
            strcat(buf, "]");
            break ;
        }
        case CG_OP_NUL:
        default:
            break;
    }
    return (strdup(buf));
}

String  cond_codes[] = {"ne", "e "};

void
CG_dump(void)
{
    BLOC_FOREACH(Instr, current, chain)
    {
        switch (current->type)
        {
            case CG_INSTR_PUSH:
                EMIT("  push    %s", current->op1.base);
                break ;
            case CG_INSTR_POP:
                EMIT("  pop     %s", current->op1.base);
                break ;
            case CG_INSTR_MOV:
            {
                String  dst = CG_operand(&current->op1, true);
                String  src = CG_operand(&current->op2, false);

                EMIT("  mov     %s, %s", dst, src);
                free((void *)dst);
                free((void *)src);
                break ;
            }
            case CG_INSTR_MOVZX:
            {
                String  dst = CG_operand(&current->op1, true);
                String  src = CG_operand(&current->op2, true);

                EMIT("  movzx   %s, %s", dst, src);
                free((void *)dst);
                free((void *)src);
                break ;
            }
            case CG_INSTR_ADD:
            {
                String  dst = CG_operand(&current->op1, true);
                String  src = CG_operand(&current->op2, false);

                EMIT("  add     %s, %s", dst, src);
                free((void *)dst);
                free((void *)src);
                break ;
            }
            case CG_INSTR_SUB:
            {
                String  dst = CG_operand(&current->op1, true);
                String  src = CG_operand(&current->op2, false);

                EMIT("  sub     %s, %s", dst, src);
                free((void *)dst);
                free((void *)src);
                break ;
            }
            case CG_INSTR_TEST:
                EMIT("  test    %s, %s", current->op1.label, current->op1.label);
                free((void *)current->op1.label);
                break ;
            case CG_INSTR_CALL:
                EMIT("  call    [%s]", current->op1.label);
                break ;
            case CG_INSTR_RET:
                EMIT("  ret");
                break ;
            case CG_INSTR_JMP:
                EMIT("  jmp     %s", current->op1.label);
                free((void *)current->op1.label);
                break ;
            case CG_INSTR_JCC:
                EMIT("  j%s     %s", cond_codes[current->op2.immediate], current->op1.label);
                free((void *)current->op1.label);
                break ;
            case CG_INSTR_DIRECTIVE:
                EMIT("%s", current->op1.label);
                free((void *)current->op1.label);
                break ;
            default:
                break ;
        }
    }
    BLOC_DESTROY(chain);
}


void
CG_directive_syntax(void)
{
	String	result;

	result = CG_sprintf(".intel_syntax noprefix");
	CG_instr(
		CG_INSTR_DIRECTIVE,
		.op1 = OP_LBL(strdup(result))
	);
}

void
CG_directive_section(String section)
{
	if (!section)
		section = ".text";
	String	result;

	result = CG_sprintf(".section %s", section);
	CG_instr(
		CG_INSTR_DIRECTIVE,
		.op1 = OP_LBL(strdup(result))
	);
}

void
CG_directive_globl(String name)
{
	if (!name)
		return ;
	String	result;

	result = CG_sprintf(".globl %s", name);
	CG_instr(
		CG_INSTR_DIRECTIVE,
		.op1 = OP_LBL(strdup(result))
	);
}

void
CG_directive_long(int32_t value, String symbol)
{
	if (!symbol)
		return ;
	String	result;

	result = CG_sprintf(".long \"%s\" + %d", symbol, value);
	CG_instr(
		CG_INSTR_DIRECTIVE,
		.op1 = OP_LBL(strdup(result))
	);
}

void
CG_directive_string(String text)
{
	String	result;

	result = CG_sprintf(".string \"%s\"", text);
	CG_instr(
		CG_INSTR_DIRECTIVE,
		.op1 = OP_LBL(strdup(result))
	);
}

void
CG_directive_set_len(String name)
{
	String	result;

	result = CG_sprintf(".set %ss, .%se - .%s - %d", name, name, name, WORD_SIZE + 1);
	CG_instr(
		CG_INSTR_DIRECTIVE,
		.op1 = OP_LBL(strdup(result))
	);
}

void
CG_label(String name)
{
	if (!name)
		return ;
	String	result;

	result = CG_sprintf("%s:", name);
	CG_instr(
		CG_INSTR_DIRECTIVE,
		.op1 = OP_LBL(strdup(result))
	);
}

void
CG_function_start(String name)
{
	CG_instr(
        CG_INSTR_PUSH,
        .op1 = OP_REG(EBP),
    );
    CG_instr(
        CG_INSTR_MOV,
        .op1 = OP_REG(EBP),
        .op2 = OP_REG(ESP),
    );
}

void
CG_function_stop(void)
{
	CG_instr(
        CG_INSTR_MOV,
        .op1 = OP_REG(ESP),
        .op2 = OP_REG(EBP),
    );
    CG_instr(
        CG_INSTR_POP,
        .op1 = OP_REG(EBP),
    );
    CG_instr(CG_INSTR_RET);
}

void
CG_stack_sub(Size size)
{
    CG_instr(
        CG_INSTR_SUB,
        .op1 = OP_REG(ESP),
        .op2 = OP_IMM(size),
    );
}

void
CG_jump(String func, String lbl)
{
    CG_sprintf(".%s.%s", func, lbl);

    CG_instr(
        CG_INSTR_JMP,
        .op1 = OP_LBL(strdup(cgtmp)),
    );
}

#define JMP_NZ  0
#define JMP_Z   1

void
CG_jump_nz(String func, String lbl)
{
    CG_sprintf(".%s.%s", func, lbl);

    CG_instr(
        CG_INSTR_JCC,
        .op1 = OP_LBL(strdup(cgtmp)),
        .op2 = OP_IMM(JMP_NZ),
    );
}

void
CG_jump_z(String func, String lbl)
{
    CG_sprintf(".%s.%s", func, lbl);

    CG_instr(
        CG_INSTR_JCC,
        .op1 = OP_LBL(strdup(cgtmp)),
        .op2 = OP_IMM(JMP_Z),
    );
}

void
CG_return_immediate(u64 imm)
{
    CG_instr(
        CG_INSTR_MOV,
        .op1 = OP_REG(EAX),
        .op2 = OP_IMM(imm),
    );
}

void
CG_return_register(Register reg)
{
    if (!strcmp(reg, EAX))
        return ;
    CG_instr(
        CG_INSTR_MOV,
        .op1 = OP_REG(EAX),
        .op2 = OP_REG(reg),
    );
}

void
CG_return_variable(Offset off)
{
    CG_instr(
        CG_INSTR_MOV,
        .op1 = OP_REG(EAX),
        .op2 = OP_MEM(
            .base = EBP,
            .displacement = off,
        ),
    );
}

void
CG_load_var(Register reg, Offset off)
{
    CG_instr(
        CG_INSTR_MOV,
        .op1 = OP_REG(reg),
        .op2 = OP_MEM(
            .base = EBP,
            .displacement = off,
        ),
    );
}

void
CG_store_immediate(Register reg, u64 imm)
{
    CG_instr(
        CG_INSTR_MOV,
        .op1 = OP_REG(reg),
        .op2 = OP_IMM(imm),
    );
}

void
CG_store_var_immediate(Offset off, i64 imm)
{
    CG_instr(
        CG_INSTR_MOV,
        .op1 = OP_MEM(
            .base = EBP,
            .displacement = off,
        ),
        .op2 = OP_IMM(imm),
    );
}

void
CG_store_var_register(Offset off, Register reg)
{
    CG_instr(
        CG_INSTR_MOV,
        .op1 = OP_MEM(
            .base = EBP,
            .displacement = off,
        ),
        .op2 = OP_REG(reg),
    );
}

void
CG_store_arg_immediate(Offset off, i64 imm)
{
    CG_instr(
        CG_INSTR_MOV,
        .op1 = OP_MEM(
            .base = ESP,
            .displacement = off,
        ),
        .op2 = OP_IMM(imm),
    );
}

void
CG_store_arg_register(Offset off, Register reg)
{
    CG_instr(
        CG_INSTR_MOV,
        .op1 = OP_MEM(
            .base = ESP,
            .displacement = off,
        ),
        .op2 = OP_REG(reg),
    );
}

void
CG_add_reg_imm(Register reg, u64 imm)
{
    CG_instr(
        CG_INSTR_ADD,
        .op1 = OP_REG(reg),
        .op2 = OP_IMM(imm),
    );
}

void
CG_add_reg_var(Register reg, Offset off)
{
    CG_instr(
        CG_INSTR_ADD,
        .op1 = OP_REG(reg),
        .op2 = OP_MEM(
            .base = EBP,
            .displacement = off
        ),
    );
}

void
CG_add_reg_reg(Register a, Register b)
{
    CG_instr(
        CG_INSTR_ADD,
        .op1 = OP_REG(a),
        .op2 = OP_REG(b),
    );
}

void
CG_sub_reg_imm(Register reg, u64 imm)
{
    CG_instr(
        CG_INSTR_SUB,
        .op1 = OP_REG(reg),
        .op2 = OP_IMM(imm),
    );
}

void
CG_sub_reg_var(Register reg, Offset off)
{
    CG_instr(
        CG_INSTR_SUB,
        .op1 = OP_REG(reg),
        .op2 = OP_MEM(
            .base = EBP,
            .displacement = off
        ),
    );
}

void
CG_sub_reg_reg(Register a, Register b)
{
    CG_instr(
        CG_INSTR_SUB,
        .op1 = OP_REG(a),
        .op2 = OP_REG(b),
    );
}

void
CG_test_z(Register reg)
{
    CG_instr(
        CG_INSTR_TEST,
        .op1 = OP_REG(reg),
    );
}

void
CG_load_byte_reg(Register dst, Register str, Register idx)
{
    CG_instr(
        CG_INSTR_MOVZX,
        .op1 = OP_REG(dst),
        .op2 = OP_MEM(
            .base = str,
            .index = idx,
            .size = 0,
        ),
    );
}

void
CG_load_byte_imm(Register dst, Register str, i64 imm)
{
    CG_instr(
        CG_INSTR_MOVZX,
        .op1 = OP_REG(dst),
        .op2 = OP_MEM(
            .base = str,
            .displacement = imm,
            .size = 0,
        ),
    );
}

void
CG_subscript_imm(Register arr, i64 imm)
{
    CG_instr(
        CG_INSTR_MOV,
        .op1 = OP_REG(arr),
        .op2 = OP_MEM(
            .base = arr,
            .displacement = imm * WORD_SIZE,
        ),
    );
}

void
CG_subscript_reg(Register arr, Register reg)
{
    CG_instr(
        CG_INSTR_MOV,
        .op1 = OP_REG(arr),
        .op2 = OP_MEM(
            .base = arr,
            .index = reg,
            .scale = 4,
        ),
    );
}

void
CG_function_call(String name)
{
    CG_instr(
        CG_INSTR_CALL,
        .op1 = OP_LBL(name),
    );
}

#endif
