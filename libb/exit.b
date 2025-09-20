exit(code)
{
	extrn	syscall;

	syscall(1, code);
}
