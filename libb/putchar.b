putchar(c)
{
	extrn syscall;

	return (syscall(4, 1, &c, 1));
}
