strlen(s)
{
	auto	i;

	i = 0;
	while (char(s, i)) ++i;
	return (i);
}

putstr(s)
{
	extrn	syscall;

	syscall(4, 1, s, strlen(s));
}

fatal(msg)
{
	extrn	exit;

	putstr("[B] ");
	putstr(msg);
	putstr("\n");
	exit(1);
}

shift(ac, av)
{
	auto	old_av;

	--(*ac);
	old_av = *av;
	*av =+ 4;

	return (*old_av);
}

mmap(addr, size, prot, flags, fd, offset)
{
	extrn	syscall;

	return (syscall(90));
}

puthex(x)
{
	while (x)
	{
		auto	hexit;

		hexit = (x & 15);
		hexit =+ hexit >= 10 ? 'a' : '0';
		x =>> 4;
		putstr(&hexit);
	}
}

main(ac, av, env)
{
	auto	exe, input;
	extrn	lexer_init;

	exe = shift(&ac, &av);
	if (!ac)
		fatal("Usage: ./B \033[3mfile\033[0m");
	
	input = shift(&ac, &av);
	if (lexer_init(input))
		fatal("No such file or directory.");

	return (0);
}
