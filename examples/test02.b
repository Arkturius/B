stdin		0;
stdout		1;
stderr		2;

total		0;
board[3]	0x30303030, 0x30303030, 0xa3030;

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

	syscall(4, stdout, s, strlen(s));
}

verif(depth, new)
{
	auto	col, c;

	col = 0;
	while (col < depth)
	{
		c = char(board, col) - 48;
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

rec_queens(depth)
{
	auto	i;

	i = 0;
	while (i < 10)
	{
		if (verif(depth, i))
		{
			lchar(board, depth, i + 48);
			if (depth == 9)
			{
				putstr(board);
				++total;
			}
			else
				rec_queens(depth + 1);
		}
		++i;
	}
}

main(argc, argv, envp)
{
	rec_queens(0);
	return (total);
}
