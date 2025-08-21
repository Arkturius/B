/**
 * bsymbol.c
 */

# include <string.h>

# include <bsymbol.h>

static inline Offset
B_symbol_offset(SymbolType type)
{
    switch (type)
    {
        case SYM_VARIABLE:
        {
            Scope   *where = vec_last(B.scopes);

            where->stack_offset += WORD_SIZE;
            return (where->stack_offset);
        }
        case SYM_PARAMETER:
        {
            B.function.args_count++;
            return (-(B.function.args_count + 1) * WORD_SIZE);
        }
        case SYM_FUNCTION:
        case SYM_EXTERNAL:
        case SYM_LABEL:
            return (0);
    }
}

void
B_symbol_new(String name, SymbolType type)
{
    Symbol  new = {0};

    new = (Symbol)
    {
        .name = name,
        .type = type,
        .off = B_symbol_offset(type),
    };
    vec_append(B.symtab, new);
}

Symbol
*B_symbol_lookup(String name)
{
    Size    end = vec_count(B.symtab);

    vec_foreach_rev(Scope, sc, B.scopes)
    {
        Size    start = sc->sym_offset;

        for (Size i = 0; i < end; ++i)
        {
            Symbol  *sym = B.symtab.items + i;

            if (!strcmp(sym->name, name))
                return (sym);
        }
        end = start;
    }
    vec_foreach(Symbol, sym, B.symtab)
    {
        if (!strcmp(sym->name, name))
            return (sym);
        if (!strcmp(sym->name, B.function.name))
            break ;
    }
    return (NULL);
}

void
B_symbol_dump(void)
{
    String  type;

    dprintf(2, "[B] symbol table:\n");
    vec_foreach(Symbol, sym, B.symtab)
    {
        dprintf(2, "  %3ld: { ", vec_index(B.symtab, sym));
        dprintf(2, ".type = ");
        switch (sym->type)
        {
            case SYM_VARIABLE:
				type = "SYM_VARIABLE";
				break ;
            case SYM_PARAMETER:
				type = "SYM_PARAMETER";
				break ;
            case SYM_FUNCTION:
				type = "SYM_FUNCTION";
				break ;
            case SYM_EXTERNAL:
				type = "SYM_EXTERNAL";
				break ;
            case SYM_LABEL:
				type = "SYM_LABEL";
				break ;
            default:
                type = "UNKNOWN";
        }
        dprintf(2, "%s, .name = %s, ", type, sym->name);
        dprintf(2, ".off = %d }\n", sym->off);
    }
}

void
B_scope_enter(void)
{
    Scope   new;

    new = (Scope)
    {
        .sym_offset = vec_count(B.symtab),
        .stack_offset =
            vec_count(B.scopes) > 0
            ? vec_last(B.scopes)->stack_offset
            : 0
    };
    vec_append(B.scopes, new);
}

void
B_scope_exit(void)
{
    B.scopes.count--;
}
