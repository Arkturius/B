read(fd, buf, len)
{
	extrn	syscall;

	return (syscall(0x03, fd, buf, len));
}
