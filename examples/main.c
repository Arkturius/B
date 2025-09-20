/**
 * main.b
 *********/

strlen(s)
{
	auto	i;

	i = 0;
	while (char(s, i)) i++;
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
	auto	self_name, input_file;

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
	}

	error("TODO: open the file\n", 1);

	return (0);
}
