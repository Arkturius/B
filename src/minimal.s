.intel_syntax noprefix

.section .text

.globl main
main:
    .long   "main" + 4
	push    ebp
	mov     ebp, esp
	sub     esp, 0x10
	mov     DWORD PTR [ebp - 0x10], 4
	mov     DWORD PTR [ebp - 0xc], 1
	mov     DWORD PTR [ebp - 0x8], OFFSET .rostr000
	mov     DWORD PTR [ebp - 0x4], OFFSET rostr000s
	call    [syscall]
	xor     eax, eax
.main.end:
	leave
	ret

.section .rodata

.rostr000:
    .long   ".rostr000" + 4
	.string "Hello World\n"
.rostr000e:
.set rostr000s, .rostr000e - .rostr000

