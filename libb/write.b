write(fd, buf, len)
{
	extrn	syscall;

	return (syscall(0x04, fd, buf, len));
}
