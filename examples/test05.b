putchar(c)
{
	extrn syscall;

	return (syscall(4, 1, &c, 1));
}

main()
{
	extrn putchar;
	auto i, s;
	i = 0;
	s = "hello, world\n";
	while (char(s, i))
	{
		putchar(char(s, i));
		i++;
	}
}
