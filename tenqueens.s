.intel_syntax noprefix
.section .text

.align 16
putchar:
.long putchar + 4
  push    ebp
  mov     ebp, esp
  sub     esp, 12
  lea     edx, [ebp + 8]
  sub     esp, 16
  mov     DWORD PTR [esp + 12], 1
  mov     DWORD PTR [esp + 8], edx
  mov     DWORD PTR [esp + 4], 1
  mov     DWORD PTR [esp], 4
  call    [syscall]
  add     esp, 16
  jmp     .L0
.L0:
  mov     esp, ebp
  pop     ebp
  ret     
.global putchar

.align 16
display:
.long display + 4
  push    ebp
  mov     ebp, esp
  sub     esp, 12
  sub     esp, 8
  mov     DWORD PTR [ebp - 4], 0
.L2:
  cmp     DWORD PTR [ebp - 4], 10
  jge     .L3
  mov     edx, [ebp + 8]
  mov     eax, [ebp - 4]
  movzx   ecx, BYTE PTR [edx + eax]
  mov     DWORD PTR [ebp - 8], ecx
  mov     edx, [ebp - 8]
  add     edx, 48
  sub     esp, 4
  mov     DWORD PTR [esp], edx
  call    [putchar]
  add     esp, 4
  add     DWORD PTR [ebp - 4], 1
  jmp     .L2
.L3:
  sub     esp, 4
  mov     DWORD PTR [esp], 10
  call    [putchar]
  add     esp, 4
.L1:
  mov     esp, ebp
  pop     ebp
  ret     
.global display

.align 16
verif:
.long verif + 4
  push    ebp
  mov     ebp, esp
  sub     esp, 12
  sub     esp, 4
  sub     esp, 4
  mov     DWORD PTR [ebp - 4], 0
.L5:
  mov     edx, [ebp - 4]
  cmp     edx, [ebp + 12]
  jge     .L6
  mov     edx, [ebp + 8]
  mov     eax, [ebp - 4]
  movzx   ecx, BYTE PTR [edx + eax]
  mov     DWORD PTR [ebp - 8], ecx
  mov     edx, [ebp - 8]
  cmp     edx, [ebp + 16]
  jne     .L7
  xor     eax, eax
  jmp     .L4
.L7:
  mov     edx, [ebp + 12]
  sub     edx, [ebp - 4]
  mov     eax, [ebp + 16]
  add     eax, edx
  cmp     DWORD PTR [ebp - 8], eax
  jne     .L8
  xor     eax, eax
  jmp     .L4
.L8:
  mov     edx, [ebp + 12]
  sub     edx, [ebp - 4]
  mov     eax, [ebp + 16]
  sub     eax, edx
  cmp     DWORD PTR [ebp - 8], eax
  jne     .L9
  xor     eax, eax
  jmp     .L4
.L9:
  add     DWORD PTR [ebp - 4], 1
  jmp     .L5
.L6:
  mov     eax, 1
  jmp     .L4
.L4:
  mov     esp, ebp
  pop     ebp
  ret     
.global verif

.align 16
rec_queens:
.long rec_queens + 4
  push    ebp
  mov     ebp, esp
  sub     esp, 12
  sub     esp, 4
  sub     esp, 4
  mov     DWORD PTR [ebp - 4], 0
  mov     DWORD PTR [ebp - 8], 0
.L11:
  cmp     DWORD PTR [ebp - 4], 10
  jge     .L12
  sub     esp, 12
  mov     edx, [ebp - 4]
  mov     DWORD PTR [esp + 8], edx
  mov     eax, [ebp + 8]
  mov     DWORD PTR [esp + 4], eax
  mov     ecx, [ebp + 12]
  mov     DWORD PTR [esp], ecx
  call    [verif]
  add     esp, 12
  test    eax, eax
  je      .L13
  mov     edx, [ebp + 12]
  mov     eax, [ebp + 8]
  mov     ecx, [ebp - 4]
  and     ecx, 255
  mov     BYTE PTR [edx + eax], cl
  cmp     DWORD PTR [ebp + 8], 9
  jne     .L14
  sub     esp, 4
  mov     edx, [ebp + 12]
  mov     DWORD PTR [esp], edx
  call    [display]
  add     esp, 4
  mov     eax, 1
  jmp     .L10
  jmp     .L15
.L14:
  mov     edx, [ebp + 8]
  add     edx, 1
  sub     esp, 8
  mov     eax, [ebp + 12]
  mov     DWORD PTR [esp + 4], eax
  mov     DWORD PTR [esp], edx
  call    [rec_queens]
  add     esp, 8
  add     DWORD PTR [ebp - 8], eax
.L15:
.L13:
  add     DWORD PTR [ebp - 4], 1
  jmp     .L11
.L12:
  mov     eax, [ebp - 8]
  jmp     .L10
.L10:
  mov     esp, ebp
  pop     ebp
  ret     
.global rec_queens

.align 16
main:
.long main + 4
  push    ebp
  mov     ebp, esp
  sub     esp, 12
  sub     esp, 16
  lea     edx, [ebp - 12]
  mov     DWORD PTR [edx], 0
  lea     edx, [ebp - 12]
  mov     DWORD PTR [edx + 4], 0
  lea     edx, [ebp - 12]
  mov     DWORD PTR [edx + 8], 0
  lea     edx, [ebp - 12]
  sub     esp, 8
  mov     DWORD PTR [esp + 4], edx
  mov     DWORD PTR [esp], 0
  call    [rec_queens]
  add     esp, 8
  mov     DWORD PTR [ebp - 16], eax
  mov     eax, [ebp - 16]
  jmp     .L16
.L16:
  mov     esp, ebp
  pop     ebp
  ret     
.global main

.section .rodata
