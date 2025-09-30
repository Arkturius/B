putchar(c)
{
	extrn syscall;

	return (syscall(4, 1, &c, 1));
}

display(rv)
{
	auto	i, c;

	i = 0;
	while (i < 10)
	{
		c = char(rv, i);
		putchar(c + '0');
		++i;
	}
	putchar(10);
}

verif(rv, depth, new)
{
	auto	col;
	auto	c;

	col = 0;
	while (col < depth)
	{
		c = char(rv, col);
		if (c == new)
			return (0);
		if (c == new + (depth - col))
			return (0);
		if (c == new - (depth - col))
			return (0);
		++col;
	}
	return (1);
}

rec_queens(depth, rv)
{
	auto	i;
	auto	total;

	i = 0;
	total = 0;
	while (i < 10)
	{
		if (verif(rv, depth, i))
		{
			lchar(rv, depth, i);
			if (depth == 9)
			{
				display(rv);
				return (1);
			}
			else
				total =+ rec_queens(depth + 1, rv);
		}
		++i;
	}
	return (total);
}

main(argc, argv, envp)
{
	auto	rv[3], n;

	rv[0] = 0;
	rv[1] = 0;
	rv[2] = 0;
	n = rec_queens(0, rv);
	return (n);
}
