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

r_puthex(x)
{
	auto	hexit, rest;

	rest = x >> 4;
	hexit = (x & 15);
	hexit =+ hexit >= 10 ? 'a' - 10 : '0';
	if (rest)
		r_puthex(rest);
	putstr(&hexit);
}

puthex(x)
{
	putstr("0x");
	r_puthex(x);
	putstr("\n");
}

lexer_fd;

lexer_map_file(size)
{
	extrn	syscall;

	return (syscall(192, 0x10000000, size, 0x03, 0x2, lexer_fd, 0));
}

lexer_open(filename)
{
	extrn	open;

	return (open(filename, 1));
}

lexer_init(filename)
{
	extrn	open;

	lexer_fd = open(filename, 0);
	return (lexer_fd < 0);
}

main(ac, av, env)
{
	extrn	fstat;
	auto	exe, input, v 20, file_ptr;

	exe = shift(&ac, &av);
	if (!ac)
		fatal("Usage: ./B \033[3mfile\033[0m");
	
	input = shift(&ac, &av);
	if (lexer_init(input))
		fatal("No such file or directory.");

	fstat(lexer_fd, v);

	file_ptr = lexer_map_file(v[7]);

	putstr("Mapped file:\n");
	puthex(file_ptr);
	puthex(&file_ptr);

	return (0);
}
