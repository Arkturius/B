.align 16
.intel_syntax noprefix
.section .text

lchar:
.long lchar + 4
  mov     eax, [ebp + 0x08]
  add     eax, [ebp + 0x0c]
  mov     edx, [ebp + 0x10]
  mov     BYTE PTR [eax], dl
  ret
.global lchar
