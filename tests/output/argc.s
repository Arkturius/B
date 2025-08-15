.intel_syntax noprefix
.section .text
.globl main
main:
    .long "main" + 4
    push    ebp
    mov     ebp, esp
returning expr
.main.end:
    pop     ebp
    ret
