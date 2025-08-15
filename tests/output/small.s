.intel_syntax noprefix
.section .text
.globl main
main:
    .long "main" + 4
    push    ebp
    mov     ebp, esp
    jmp     
simple return
.main.end:
    pop     ebp
    ret
