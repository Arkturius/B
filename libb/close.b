close(fd)
{
	extrn syscall;

	return (syscall(3, fd));
}
