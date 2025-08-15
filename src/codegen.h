/**
 * codegen.h
 */

#ifndef _CODEGEN_H
# define _CODEGEN_H

# include <string.h>
# include <stdio.h>
# include <types.h>

# define	EAX	"eax"
# define	EBX	"ebx"
# define	ECX	"ecx"
# define	EDX	"edx"
# define	ESP	"esp"
# define	EBP	"ebp"
# define	EDI	"edi"
# define	ESI	"esi"

# define	EMIT(X, ...)		printf(X"\n", ##__VA_ARGS__)
# define	ARG_OP(O, X, ...)	EMIT("    %s%*.s"X, O, (int)(8 - strlen(O)), "", ##__VA_ARGS__);
# define	NOARG_OP(O)			EMIT("    %s", O)

# define	PUSH(X)			ARG_OP("push", X)
# define	POP(X)			ARG_OP("pop", X)

# define	MOV(D, S)		ARG_OP("mov", "%s, %s", D, S)

# define	RET				NOARG_OP("ret")
# define	JMP(L)			ARG_OP("jmp", L)

void
CG_directive_syntax(void)
{
	EMIT(".intel_syntax noprefix");
}

void
CG_directive_section(String section)
{
	if (!section)
		section = ".text";
	EMIT(".section %s", section);
}

void
CG_directive_globl(String name)
{
	if (!name)
		return ;
	EMIT(".globl %s", name);
}

void
CG_directive_long(int32_t value, String symbol)
{
	if (!symbol)
		return ;
	EMIT("    .long \"%s\" + %d", symbol, value);
}

void
CG_directive_string(String text)
{
	EMIT("    .string \"%s\\n\"", text);
}

void
CG_directive_set_len(String name)
{
	EMIT(".set %ss, .%se - .%s - %d", name, name, name, WORD_SIZE + 1);
}

void
CG_label(String name)
{
	if (!name)
		return ;
	EMIT("%s:", name);
}

void
CG_function_start(String name)
{
	PUSH(EBP);
	MOV(EBP, ESP);
}

void
CG_function_stop(void)
{
	POP(EBP);
	RET;
}

#endif
