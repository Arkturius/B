/**
 * main.b
 *********/

strlen(s)
{
	auto	i;

	i = 0;
	while (char(s, i)) ++i;
	return (i);
}

printf(s)
{
	auto	len;
	extrn	syscall;

	len = strlen(s);
	return (syscall(4, 1, s, len));
}

exit(code)
{
	extrn	syscall;

	syscall(1, code);
}

error(msg, code)
{
	printf(msg);
	exit(code);
}

shift(ac, av)
{
	auto	old_av;

	(*ac)--;
	old_av = *av;
	*av =+ 4;
	return (*old_av);
}

main(ac, av, env)
{
	extrn	open, printn;
	auto	self_name, input_file, input_fd;

	self_name = shift(&ac, &av);
	printf("[B] compiler exe: ");
	printf(self_name);
	printf("\n");

	if (ac)
	{
 		input_file = shift(&ac, &av);
		printf("[B] input file  : ");
		printf(input_file);
		printf("\n");
		if (ac) error("[B] too many arguments.\n");
	}
	else error("[B] no file provided.\n");

	input_fd = open(input_file, 0);
	if (input_fd == -1) error("[B] no such file or directory.\n");

	printn(input_fd, 10);

	return (0);
}
