chmod(filename, mode)
{
	extern syscall;

	return (syscall(0x5a, filename, mode));
}
