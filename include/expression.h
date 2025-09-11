/**
 * expression.h
 */

#if !defined (_EXPRESSION_H)
# define _EXPRESSION_H

# include <types.h>
# include <symbols.h>

# define	REG(_s)	((Expression){.type = EXPR_REGISTER, .reg = _s})
# define	IMM(_i)	((Expression){.type = EXPR_IMMEDIATE, .imm = _i})

typedef enum _bregister
{
	REG_EDX,
	REG_EAX,
	REG_ECX,
	REG_EBX,
	REG_EDI,
	REG_ESI,
	REG_USABLE = REG_ESI,
	REG_ESP,
	REG_EBP,

	REG_ENUM_MAX,
}	Register;

extern StringC	register_names[REG_ENUM_MAX];
# define		REGISTER(_d)	register_names[(_d)]

typedef struct	_bmemory	Memory;

struct _bmemory
{
	Register	base;
	Register	index;
	u32			scale;
	u32			displacement;
};

# define	MEM(...)		(Memory){ __VA_ARGS__ }
# define	MEM_STACK(_o)	MEM( .base = REG_EBP, .displacement = _o )

typedef enum _bexpr_type
{
	EXPR_REGISTER,
	EXPR_MEMORY,
	EXPR_IMMEDIATE,
}	ExpressionType;

struct _bexpression
{
	ExpressionType	type;
	union
	{
		u32			imm;
		Register	reg;
	};
	Memory			mem;
};

enum _bassign_type
{
    ASSIGN_OP,
    ASSIGN_OP_EQ,
    ASSIGN_OP_NE,
    ASSIGN_OP_LE,
    ASSIGN_OP_GE,
    ASSIGN_OP_LT,
    ASSIGN_OP_GT,
    ASSIGN_OP_LSHIFT,
    ASSIGN_OP_RSHIFT,
    ASSIGN_OP_OR,
    ASSIGN_OP_AND,
    ASSIGN_OP_PLUS,
    ASSIGN_OP_MINUS,
    ASSIGN_OP_MULT,
    ASSIGN_OP_DIV,
    ASSIGN_OP_MOD,
};

#endif // _EXPRESSION_H
