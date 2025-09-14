strlen(s)
{
    auto i;
    
    i = 0;
    while (char(s, i))
		i =+ 1;
	return (i);
}

main(argc, argv, envp)
{
	auto	a;

	a = 2;
	a(a);
    return (strlen(argv[1]) + 42);
}

