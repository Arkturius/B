/**
 * codegen.h
 */

#if !defined (_CODEGEN_H)
# define _CODEGEN_H

# include <b.h>

typedef enum _bsyntax_type
{
	SYNTAX_INTEL,

	SYNTAX_ENUM_MAX,
}	SyntaxType;

typedef enum _bdirective_type
{
	DIRECTIVE_SYNTAX,
	DIRECTIVE_ALIGN,
	DIRECTIVE_SECTION,
	DIRECTIVE_GLOBAL,
	DIRECTIVE_LONG,

	DIRECTIVE_ENUM_MAX,
}	DirectiveType;

typedef enum _bsection_type
{
	SECTION_TEXT,
	SECTION_DATA,
	SECTION_RODATA,
	SECTION_BSS,

	SECTION_ENUM_MAX,
}	SectionType;

# define	REG(_s)	((Expression){.type = EXPR_REGISTER, .reg = _s})

# define	REG_EAX	REG("eax")
# define	REG_EBX	REG("ebx")
# define	REG_ECX	REG("ecx")
# define	REG_EDX	REG("edx")
# define	REG_EDI	REG("edi")
# define	REG_ESI	REG("esi")
# define	REG_ESP	REG("esp")
# define	REG_EBP	REG("ebp")

extern StringC	syntax_names[SYNTAX_ENUM_MAX];
# define		SYNTAX(_d)	syntax_names[(_d)]

extern StringC	directive_names[DIRECTIVE_ENUM_MAX];
# define		DIRECTIVE(_d)	directive_names[(_d)]

extern StringC	section_names[SECTION_ENUM_MAX];
# define		SECTION(_s)		section_names[(_s)]


struct _asm_directive_opt
{
	union
	{
		const u64	value;
		const void	*data;
	};
	const u32	off;
};

void
asm_directive_opt(DirectiveType type, struct _asm_directive_opt opt);

# define	asm_directive(_type, ...)	\
	asm_directive_opt(_type, (struct _asm_directive_opt){ __VA_ARGS__ })

void
asm_label(String name);

void
asm_push(Expression p);

void
asm_pop(Expression p);

void
asm_mov(Expression dst, Expression src);

void
asm_xor(Expression a, Expression b);

void
asm_ret(void);

#endif // _CODEGEN_H
