main()
{
	extrn putchar;
	auto i, s;
	i = 0;
	s = 2;
	while (char(s, i))
	{
		putchar(char(s, i));
		i++;
	}
}
