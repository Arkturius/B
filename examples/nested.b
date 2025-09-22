
foo(a)
{
	return (a + a);
}

main(ac, av, env)
{
	auto a;

	a = foo(1) + foo(2) + foo(3);
	return (a);
}
