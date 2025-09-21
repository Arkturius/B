putchar(c)
{
	extrn syscall;

	return (syscall(4, 1, &c, 1));
}

inc(px, x)
{
	(*px) =+ x;
}

putn(n)
{
	auto	save;

	save = n;
	while (n > 10)
	{
		putn(n / 10);
		break ;
	}
	putchar(n % 10);
}

main(argc, argv, envp)
{
	auto	v[9], l, ll;

	l = 9;
	while (l)
	{
		ll = l + 1;
		while (--ll)
			inc(&v[l], ll);
		putn(v[l]);
		putchar(10);
		--l;
	}
	return (0);
}
