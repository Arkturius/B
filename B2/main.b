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

	putstr("[ERROR]: ");
	putstr(msg);
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

lexer_fd;
lexer_input;

lexer_open(filename)
{
	extrn	open;

	return (open(filename, 1));
}

main(ac, av, env)
{
	auto	exe;

	exe = shift(&ac, &av);
	
	if (!ac)
		lexer_fd = 0;
	else
	{
		lexer_input = shift(&ac, &av);
		lexer_fd    = lexer_open(lexer_input);
	}

	if (lexer_fd < 0)
		fatal("No such file or directory.");
	return (0);
}
