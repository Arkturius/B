.align 16
.intel_syntax noprefix
.section .text

char:
.long char + 4
  mov     eax, [ebp + 0x08]
  add     eax, [ebp + 0x0c]
  movzx   eax, BYTE PTR [eax]
  ret
.global char
