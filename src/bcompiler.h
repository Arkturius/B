/**
 * B compiler
 */

#ifndef _BCOMPILER_H
# define _BCOMPILER_H

# include <stdarg.h>
# include <bparser.h>
# include <stdio.h>

# include <btypes.h>
# include <bcontext.h>
# include <codegen.h>

int yyerror(const char *s);

#endif // _BCOMPILER

#define BCOMPILER_IMPLEMENTATION
#if defined(BCOMPILER_IMPLEMENTATION)

# define BLOC_IMPLEMENTATION
# include <bloc.h>

BCompiler	B = {0};

# define    B_FILEINFO  B_sprintf("%s:%s:%d", __func__, __FILE__, __LINE__)

# if defined(B_DEBUG)
#  define	B_ERROR(X)		\
	do { \
        dprintf(2, "[ERROR] %s - %s\n", B_FILEINFO, (X)); \
        abort(); \
    while (0)
#  define	B_WARNING(X)	\
	dprintf(2, "[WARN!] %s - %s\n", B_FILEINFO, (X));
#  define	B_LOG(X, ...)	\
	dprintf(2, "[DEBUG] "X"\n", ##__VA_ARGS__);
#  define    DUMP_EXPR(_e)  \
    printf("(Expr) { .type = %d, .data = 0x%-16lx, .lval = %d }\n", _e.type, _e.imm, _e.lval);
# else
#  define   B_ERROR(X)
#  define   B_WARNING(X)
#  define   B_LOG(X, ...)
#  define   DUMP_EXPR(_e)
# endif

String
B_sprintf(const char *fmt, ...)
{
    static char btmp[1024] = {0};
	va_list	ap;

	va_start(ap, fmt);
	vsnprintf(btmp, sizeof(btmp), fmt, ap);
	va_end(ap);

	return btmp;
}

void
B_rodata(void)
{

}

void
B_program_start(void)
{
	B_LOG("%s", __func__);

	CG_directive_syntax();
	CG_directive_section(".text");
}

void
B_program_stop(void)
{
	B_LOG("%s", __func__);

	B_rodata();
}

u32
B_symbol_new(String name, SymbolType type)
{
// 	B_LOG("%s", __func__);
// 
// 	StringIdx	idx = strtab_append(bcp.symbols, name);
// 	Symbol		new = (Symbol)
// 	{	
// 		.name = idx,
// 		.type = type,
//         .off = type == SYM_VAR_STACK
//             ? (bcp.auto_count + 1) * WORD_SIZE
//             : type == SYM_ARG_STACK
//                 ? (bcp.args_count + 2) * WORD_SIZE
//                 : 0
// 	};
// 
//     B_LOG("NEW SYMBOL: named '%s' at idx %d, type = %d, off = %d", name, idx, type, new.off);
// 	BLOC_APPEND(bcp.symtab, new);
// 	free((void *)name);
//     return (idx);
}

// TODO : drop 'count' symbols at the end of a scope. making them "disappear" from context
void
B_symbol_drop(Size count)
{

}

Symbol
*B_symbol_find(String name)
{
	B_LOG("%s", __func__);

    
    BLOC_FOREACH(Symbol, sym, bcp.symtab)
    {
        String  sym_name = strtab_get(bcp.symbols, sym->name);

        if (!strcmp(sym_name, name))
            return (sym);
    }
    return (NULL);
}

void
B_function(String name)
{
	B_LOG("%s", __func__);

	CG_directive_globl(name);
	CG_label(name);
	CG_directive_long(WORD_SIZE, name);
	CG_function_start(name);

    bcp.function = B_symbol_new(name, SYM_FUNCTION);
}

void
B_function_end()
{
	B_LOG("%s", __func__);

	String	name = strtab_get(bcp.symbols, bcp.function);
	String	end_label = B_sprintf(".%s.end", name);

	CG_label(end_label);
	CG_function_stop();

    bcp.auto_count = 0;
    bcp.args_count = 0;
}

void
B_function_param(String name)
{
	B_LOG("%s", __func__);

    B_symbol_new(name, SYM_ARG_STACK);
    bcp.args_count++;
}

void
B_function_arg(Expr a)
{
	B_LOG("%s", __func__);

    Register    reg = NULL;
    Offset      off = bcp.call_count * WORD_SIZE;

    switch (a.type)
    {
        case B_EXPR_VARIABLE:
            reg = CG_register_alloc();
            CG_load_var(reg, a.off);
            a.reg = reg;
        /* fallthrough */
        case B_EXPR_REGISTER:
            CG_store_arg_register(off, a.reg);
            break ;
        case B_EXPR_IMMEDIATE:
            CG_store_arg_immediate(off, a.imm);
            break ;
        case B_EXPR_ROSTRING:
        case B_EXPR_FUNCTION:
            CG_store_arg_register(off, a.name);
            break ;
        default:
            B_ERROR("unreachable code path.");
    }
    if (reg)
        CG_register_free(reg);
    bcp.call_count++;
}

Expr
B_function_call(Expr f)
{
	B_LOG("%s", __func__);

    String  reg;

    switch (f.type)
    {
        case B_EXPR_VARIABLE:
            reg = CG_register_alloc();
            CG_load_var(reg, f.off);
        /* fallthrough */
        case B_EXPR_REGISTER:
            CG_function_call(f.reg);
            break ;
        case B_EXPR_FUNCTION:
            CG_function_call(f.name);
            break ;
        default:
            B_ERROR("not a function.");
    }

    return (Expr) { .type = B_EXPR_REGISTER, .reg = EAX, .lval = 1 };
}

void
B_auto_var(String name)
{
	B_LOG("%s", __func__);

    B_symbol_new(name, SYM_VAR_STACK);
    bcp.auto_count++;
}

void
B_auto_decl(void)
{
	B_LOG("%s", __func__);

    CG_stack_sub(bcp.auto_count * WORD_SIZE);
}

void
B_while_start()
{
	B_LOG("%s", __func__);

    String      func = strtab_get(bcp.symbols, bcp.function);
    String      loop;

    loop = B_sprintf("l%03d", bcp.loopidx++);
    bcp.loop = strtab_append(bcp.labels, loop);
    loop = strtab_get(bcp.labels, bcp.loop);
    CG_label(B_sprintf(".%s.%ss", func, loop));
}

void
B_while(Expr cond)
{
	B_LOG("%s", __func__);

    Register    reg = NULL;
    String      func = strtab_get(bcp.symbols, bcp.function);
    String      loop = strtab_get(bcp.labels, bcp.loop);

    switch (cond.type)
    {
        case B_EXPR_IMMEDIATE:
        {
            B_ERROR("while(imm) not supported.");
            break ;
        }
        case B_EXPR_VARIABLE:
        {
            reg = CG_register_alloc();
            CG_load_var(reg, cond.off);
            break ;
        }
        case B_EXPR_REGISTER:
            reg = cond.reg;
        default:
            break ;
    }
    if (reg)
    {
        CG_test_z(reg);
        CG_register_free(reg);
    }
    CG_jump_z(func, B_sprintf("%se", loop));
}

void
B_while_end(void)
{
	B_LOG("%s", __func__);

    String  func = strtab_get(bcp.symbols, bcp.function);
    String  loop = strtab_get(bcp.labels, bcp.loop);
    
    CG_jump(func, B_sprintf("%ss", loop));
    CG_label(B_sprintf(".%s.%se", func, loop));
}

void
B_return(void)
{
	B_LOG("%s", __func__);

    String  func = strtab_get(bcp.symbols, bcp.function);

	CG_jump(func, "end");
	printf("simple return\n");
}

void
B_expr_return(Expr r)
{
	B_LOG("%s", __func__);

    switch (r.type)
    {
        case B_EXPR_IMMEDIATE:
            CG_return_immediate(r.imm);
			break ;
        case B_EXPR_REGISTER:
            CG_return_register(r.reg);
			break ;
        case B_EXPR_VARIABLE:
            CG_return_variable(r.off);
			break ;
        default:
            B_ERROR("unreachable code path");
            break ;
    }
}

void
B_if_block(Expr cond) {}

void
B_if_else_block(Expr cond) {}

void
B_if_end(void) {}

Expr
B_assign(u32 type, Expr lhs, Expr rhs)
{
	B_LOG("%s", __func__);

    if (!lhs.lval)
    {
        B_ERROR("lvalue is required.");
    }
    Register    reg = NULL;

    switch (type)
    {
        case ASSIGN:
        {
            switch (rhs.type)
            {
                case B_EXPR_IMMEDIATE:
                    CG_store_var_immediate(lhs.off, rhs.imm);
                    return (Expr) { .type = B_EXPR_IMMEDIATE, .imm = rhs.imm, .lval = rhs.lval };
                case B_EXPR_REGISTER:
                    CG_store_var_register(lhs.off, rhs.reg);
                    reg = rhs.reg;
                    break ;
                case B_EXPR_FUNCTION:
                case B_EXPR_VARIABLE:
                {
                    reg = CG_register_alloc();
                    
                    CG_load_var(reg, rhs.off);
                    CG_store_var_register(lhs.off, reg);
                    break ;
                }
                case B_EXPR_ROSTRING:
                {
                    B_ERROR("Todo: rostring assignment");
                    break; 
                }
            }
            break ;
        }
        default:
            B_ERROR("Unsupported assignment operator.");
    }
    if (reg)
        CG_register_free(reg);
    return (rhs);
}

Expr
B_ternary(Expr cond, Expr yes, Expr no) {}

# define    B_OP_BINARY(_op)    Expr B_binary_##_op(Expr a, Expr b)
# define    B_OP_UNARY(_op)     Expr B_unary_##_op(Expr a)
# define    B_OP_COMP(_op)      Expr B_comp_##_op(Expr a, Expr b)

# define    _UNARY(_op)     (_op - B_UNARY_BASE - 1)
# define    _COMPARE(_op)   (_op - B_COMPARATOR_BASE - 1)

typedef Expr    (*B_Op_Unary)(Expr);
typedef Expr    (*B_Op_Binary)(Expr, Expr);
typedef Expr    (*B_Op_Comparator)(Expr, Expr);

B_Op_Binary     B_Ops_Binary[];
B_Op_Unary      B_Ops_Unary[];
B_Op_Comparator B_Ops_Comparator[];

typedef enum    _b_op_type
{
    B_OP_ADD,
    B_OP_SUB,
    B_OP_MUL,
    B_OP_DIV,
    B_OP_MOD,
    B_OP_SHL,
    B_OP_SHR,
    B_OP_AND,
    B_OP_OR,
    B_OP_XOR,
    B_OP_SUBSCRIPT,

    B_UNARY_BASE,
    B_OP_DEREF,
    B_OP_ADDROF,
    B_OP_NEGATE,
    B_OP_INVERT,
    B_OP_INCR,
    B_OP_DECR,
    B_OP_PRE_INCR,
    B_OP_PRE_DECR,

    B_COMPARATOR_BASE,
    B_OP_EQUAL,
    B_OP_NOT_EQUAL,
    B_OP_LOWER,
    B_OP_LOWER_EQUAL,
    B_OP_GREATER,
    B_OP_GREATER_EQUAL,

}   OpType;

B_OP_BINARY(add)
{
	B_LOG("%s", __func__);

    String  reg = a.reg;

    switch (a.type)
    {
        case B_EXPR_VARIABLE:
        {
            reg = CG_register_alloc();
            CG_load_var(reg, a.off);
        }
        __attribute__((fallthrough));
        case B_EXPR_REGISTER:
        {
            if (b.type == B_EXPR_IMMEDIATE)
                CG_add_reg_imm(reg, b.imm);
            if (b.type == B_EXPR_VARIABLE)
                CG_add_reg_var(reg, b.off);
            if (b.type == B_EXPR_REGISTER)
                CG_add_reg_reg(reg, b.reg);
            return ((Expr){ .type = B_EXPR_REGISTER, .reg = reg });
        }
        case B_EXPR_IMMEDIATE:
            return ((Expr){ .type = B_EXPR_IMMEDIATE, .imm = a.imm + b.imm });
        default:
            break ;
    }
    B_ERROR("Unsupported 'add' operands.");

    return (Expr){0};
}

B_OP_BINARY(sub)
{
	
}

B_OP_BINARY(mul)
{
	
}

B_OP_BINARY(div)
{
	
}

B_OP_BINARY(mod)
{
	
}

B_OP_BINARY(shl)
{

}

B_OP_BINARY(shr)
{

}

B_OP_BINARY(or)
{

}

B_OP_BINARY(xor)
{

}

B_OP_BINARY(and)
{

}

Expr
B_binary_op(OpType type, Expr a, Expr b)
{
	B_LOG("%s", __func__);
    DUMP_EXPR(a);
    DUMP_EXPR(b);

    if (type >= B_UNARY_BASE) { B_ERROR("Unknown operation type."); }

    B_Op_Binary op = B_Ops_Binary[type];

    switch (type)
    {
        case B_OP_ADD:
        case B_OP_MUL:
        case B_OP_AND:
        case B_OP_OR:
        case B_OP_XOR:
        {
            if (a.type == B_EXPR_IMMEDIATE && b.type != B_EXPR_IMMEDIATE)
                { SWAP(Expr, a, b); }
            if (a.type == B_EXPR_VARIABLE && b.type == B_EXPR_REGISTER)
                { SWAP(Expr, a, b); }
            break ;
        }
        default:
            break ;
    }
    return (op(a, b));
}

B_OP_UNARY(deref)
{
	
}

B_OP_UNARY(addrof)
{
	
}

B_OP_BINARY(subscript)
{
	B_LOG("%s", __func__);

    String  arr = NULL;
    String  idx = NULL;

    switch (a.type)
    {
        case B_EXPR_IMMEDIATE:
            { B_ERROR("imm[idx] not supported."); }
        case B_EXPR_VARIABLE:
            arr = CG_register_alloc();
            CG_load_var(arr, a.off);
            a.reg = arr;
        case B_EXPR_REGISTER:
        default:
            break ;
    }
    switch (b.type)
    {
        case B_EXPR_IMMEDIATE:
            CG_subscript_imm(a.reg, b.imm);
            break ;
        case B_EXPR_VARIABLE:
            idx = CG_register_alloc();
            CG_load_var(idx, a.off);
            b.reg = idx;
        __attribute__((fallthrough));
        case B_EXPR_REGISTER:
            CG_subscript_reg(a.reg, b.reg);
            break ;
        default:
            break ;
    }

    if (idx)
        CG_register_free(idx);
    return (Expr) { .type = B_EXPR_REGISTER, .reg = arr, .lval = 1 };
}


B_OP_UNARY(negate)
{
	
}

B_OP_UNARY(invert)
{
	
}

B_OP_UNARY(incr)
{
	
}

B_OP_UNARY(decr)
{
	
}

B_OP_UNARY(pre_incr)
{
	
}

B_OP_UNARY(pre_decr)
{
	
}

B_OP_COMP(equal)
{
	
}

B_OP_COMP(not_equal)
{
	
}

B_OP_COMP(lower_than)
{
	
}

B_OP_COMP(lower_equal)
{
	
}

B_OP_COMP(greater_than)
{
	
}

B_OP_COMP(greater_equal)
{
	
}


B_Op_Binary  B_Ops_Binary[] = 
{
    [B_OP_ADD]       = B_binary_add, 
    [B_OP_SUB]       = B_binary_sub,
    [B_OP_MUL]       = B_binary_mul,
    [B_OP_DIV]       = B_binary_div,
    [B_OP_MOD]       = B_binary_mod,
    [B_OP_SHL]       = B_binary_shl,
    [B_OP_SHR]       = B_binary_shr,
    [B_OP_AND]       = B_binary_and,
    [B_OP_OR]        = B_binary_or,
    [B_OP_XOR]       = B_binary_xor,
    [B_OP_SUBSCRIPT] = B_binary_subscript,
};

B_Op_Unary  B_Ops_Unary[] = 
{
    [_UNARY(B_OP_DEREF)]    = B_unary_deref,
    [_UNARY(B_OP_ADDROF)]   = B_unary_addrof,
    [_UNARY(B_OP_NEGATE)]   = B_unary_negate,
    [_UNARY(B_OP_INVERT)]   = B_unary_invert,
    [_UNARY(B_OP_INCR)]     = B_unary_incr,
    [_UNARY(B_OP_DECR)]     = B_unary_decr,
    [_UNARY(B_OP_PRE_INCR)] = B_unary_pre_incr,
    [_UNARY(B_OP_PRE_DECR)] = B_unary_pre_decr,
};

B_Op_Comparator B_Ops_Comparator[] = 
{
    [_COMPARE(B_OP_EQUAL)]         = B_comp_equal,
    [_COMPARE(B_OP_NOT_EQUAL)]     = B_comp_not_equal,
    [_COMPARE(B_OP_LOWER)]         = B_comp_lower_than,
    [_COMPARE(B_OP_LOWER_EQUAL)]   = B_comp_lower_equal,
    [_COMPARE(B_OP_GREATER)]       = B_comp_greater_than,
    [_COMPARE(B_OP_GREATER_EQUAL)] = B_comp_greater_equal,
};

Expr
B_expr_constant(i64 number)
{
	B_LOG("%s", __func__);

    return (Expr)
    {
        .type = B_EXPR_IMMEDIATE,
        .imm = (u64) number,
        .lval = false,
    };
}

Expr
B_expr_string(String str)
{
	B_LOG("%s", __func__);
    
    return (Expr)
    {
        .type = B_EXPR_ROSTRING,
        .ptr = (char *)str,
        .lval = false,
    };
}

Expr
B_expr_char(String str) {}

Expr
B_expr_variable(String var)
{
	B_LOG("%s", __func__);

    Symbol  *sym = B_symbol_find(var);

    if (!sym)
    {
        yyerror(B_sprintf("%s variable '%s' not declared.", B_FILEINFO, var));
        B_compiler_stop(&bcp);
        abort();
    }

    if (sym->type == SYM_FUNCTION)
    {
        return (Expr)
        {
            .type = B_EXPR_FUNCTION,
            .name = var,
            .lval = false,
        };
    }
    return (Expr)
    {
        .type = B_EXPR_VARIABLE,
        .off = sym->off,
        .lval = true,
    };
}

Expr
B_builtin_char(Expr str, Expr idx)
{
	B_LOG("%s", __func__);

    DUMP_EXPR(str);
    DUMP_EXPR(idx);

    String  result = CG_register_alloc();
    String  base = NULL;
    String  off = NULL;

    switch (str.type)
    {
        case B_EXPR_VARIABLE:
            base = CG_register_alloc();
            CG_load_var(base, str.off);
            str.reg = base;
            break ;
        case B_EXPR_IMMEDIATE:
            { B_ERROR("immediate as string in char()."); }
        case B_EXPR_ROSTRING:
        case B_EXPR_REGISTER:
        default:
            break ;
    }
    switch (idx.type)
    {
        case B_EXPR_VARIABLE:
            off = CG_register_alloc();
            CG_load_var(off, idx.off);
            CG_load_byte_reg(result, base, off);
            break ;
        case B_EXPR_IMMEDIATE:
            CG_load_byte_imm(result, base, idx.imm);
            break ;
        case B_EXPR_ROSTRING:
            { B_ERROR("rostring index in char()."); }
        case B_EXPR_REGISTER:
        default:
            break ;
    }
    if (base)
        CG_register_free(base);
    if (off)
        CG_register_free(off);

    return (Expr) { .type = B_EXPR_REGISTER, .reg = result };
}

Expr
B_builtin_lchar(Expr str, Expr idx, Expr chr) {}

#endif // BCOMPILER_IMPLEMENTATION

