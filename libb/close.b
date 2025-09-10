close(fd)
{
	extern syscall;

	return (syscall(0x03, fd));
}
