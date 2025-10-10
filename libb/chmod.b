chmod(filename, mode)
{
	extrn	syscall;

	return (syscall(0x5a, filename, mode));
}
