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
	auto	v[3];

	v[0] = 12;
	v[2] = 69;
	v[1] = 0xffff;
    return (strlen(argv[1]) + v[2]);
}

