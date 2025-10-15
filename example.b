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

main(ac, av)
{
	switch (ac)
	{
		case 1:
			putstr("1 !\n");
			break ;
		case 2:
			putstr("2 !\n");
			break ;
		case 3:
			{
				putstr("3 !\n");
				switch (char(av[1], 0))
				{
					case 'a':
						putstr("AAA\n");
					case 'b':
						putstr("BBB\n");
					default:
						break ;
				}
			}
			break ;
		default:
			putstr("OTHER\n");
			break ;
	}
	return (0);
}
