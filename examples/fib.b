main(ac, av, env)
{
	auto	a, b, c;

	a = 0;
	b = 1;
	while (a < 1000000)
	{
		c = a + b;
		a = b;
		b = c;
	}
	return (42);
}


caca[4] {2, 3, 4, 5}
