/**
 * expression.h
 */

#if !defined (_EXPRESSION_H)
# define _EXPRESSION_H

# include <types.h>
# include <symbols.h>

typedef i32							Immediate;
typedef enum	_bregister			Register;
typedef struct	_bregister_state	RegState;
typedef struct	_bregister_frame	RegFrame;
typedef struct	_bmemory			Memory;
typedef struct	_bexpression		Expression;

typedef enum	_bexpr_type			ExpressionType;
typedef enum	_bbinop_type		BinopType;
typedef enum 	_bassign_type		AssignType;
typedef enum 	_bcompare_type		CompareType;

# define	IMM(_i)			((Expression){.type = EXPR_IMMEDIATE, .imm = _i})
# define	REG(_s)			((Expression){.type = EXPR_REGISTER, .reg = _s})
# define	MEM(...)		(Memory){ __VA_ARGS__ }
# define	MEM_STACK(_o)	MEM( .base = REG_EBP, .displacement = _o )
# define	SYM(_s)			((Expression){.type = EXPR_SYMBOL, .sym = _s})

typedef enum _bregister
{
	REG_NULL,
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

# define EDX	REG(REG_EDX)
# define EAX	REG(REG_EAX)
# define ECX	REG(REG_ECX)
# define EBX	REG(REG_EBX)
# define EDI	REG(REG_EDI)
# define ESI	REG(REG_ESI)
# define ESP	REG(REG_ESP)
# define EBP	REG(REG_EBP)

extern StringC	register_names[REG_ENUM_MAX];
# define	REG_NAME(_d)	register_names[(_d)]

struct _bregister_state
{
	bool	in_use;
	bool	spilled;
	Offset	spill;
};

struct _bregister_frame
{
	RegState	states[REG_USABLE + 1];
	Offset		spill;
};

void
register_frame_start(void);

void
register_frame_stop(void);

void
register_frame_push(void);

void
register_frame_pop(void);

struct _bmemory
{
	Register	base;
	Register	index;
	u32			scale;
	i32			displacement;
	u32			size;
};


enum _bexpr_type
{
	EXPR_REGISTER,
	EXPR_MEMORY,
	EXPR_IMMEDIATE,
	EXPR_SYMBOL,

	EXPR_ENUM_MAX,
};

enum _bbinop_type
{
    BINOP,
    BINOP_EQ,
    BINOP_NE,
    BINOP_LE,
    BINOP_GE,
    BINOP_LT,
    BINOP_GT,
	BINOP_COMP_ENUM_MAX,
    BINOP_LSHIFT = BINOP_COMP_ENUM_MAX,
    BINOP_RSHIFT,
    BINOP_OR,
    BINOP_AND,
    BINOP_PLUS,
    BINOP_MINUS,
    BINOP_MULT,
    BINOP_DIV,
    BINOP_MOD,
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

	ASSIGN_ENUM_MAX,
};

struct _bexpression
{
	ExpressionType	type;
	union
	{
		u32			imm;
		Register	reg;
		StringC		sym;
	};
	Memory			mem;
	BinopType		comparison;
};

arr_decl(Expression, Expressions);

extern StringC	jump_ccs[BINOP_COMP_ENUM_MAX];
# define	JCC(_j)	jump_ccs[(_j)]

Expression
B_expression_variable(StringC name);

Expression
B_expression_constant(u64 value, StringC str, bool is_char);

Expression
B_expression_assignment(AssignType type, Expression lhs, Expression rhs);

Expression
B_expression_binop(BinopType type, Expression a, Expression b);

Expression
B_expression_subscript(Expression arr, Expression idx);

Expression
B_expression_address(Expression e);

Expression
B_expression_deref(Expression e);

Expression
B_expression_incr(Expression e);

Expression
B_expression_decr(Expression e);

Expression
B_builtin_char(Expression str, Expression idx);

#endif // _EXPRESSION_H
