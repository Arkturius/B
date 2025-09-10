open(filename, mode)
{
	extern syscall;

	if (mode == 0)
		return (syscall(0x02, filename, 0))
	else
		return (syscall(0x02, filename, 1))
}
