fib(n)
{
	auto	a, b, c;

	a = 0;
	b = 1;
	while (a < n)
	{
		c = a + b;
		a = b;
		b = c;
	}
	return (a);
}

main(ac, av, env)
{
	auto	ret;

	ret = fib(1000000);

	return (ret);
}
