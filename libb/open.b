open(filename, mode)
{
	extrn syscall;

	if (mode)
		return (syscall(0x05, filename, 1));
	else
		return (syscall(0x05, filename, 0));
}
