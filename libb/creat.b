creat(filename, mode)
{
	extern syscall;

	return (syscall(0x55, filename, mode));
}
