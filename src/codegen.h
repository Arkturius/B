/**
 * codegen.h
 */

#ifndef _CODEGEN_H
# define _CODEGEN_H

# include <types.h>

void
CG_directive_syntax(void);

void
CG_directive_section(String section);

void
CG_directive_globl(String name);

void
CG_directive_long(int32_t value, String symbol);

void
CG_directive_string(String text);

void
CG_directive_set(String name, String value);


void
CG_function(String name);

void
CG_label(String name);


void
CG_string(String text);

#endif
