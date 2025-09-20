/**
 * main.b
 *********/

putchar(c)
{
	extrn syscall;

	return (syscall(4, 1, &c, 1));
}

shift(ac, av)
{
	(*ac)--;
	return (*(*av)++);
}

main(ac, av, env)
{
	extrn	putchar;
	auto	self_name, input_file;

	i = 0;
	self_name = shift(&ac, &av);

	while (char(exe, i))
	{
		putchar(char(exe, i));
		i++;
	}

	return (0);
}
