creat(filename, mode)
{
	extrn	syscall;

	return (syscall(0x55, filename, mode));
}
