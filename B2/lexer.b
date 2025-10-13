lexer_fd;

lexer_open(filename)
{
	extrn	open;

	return (open(filename, 1));
}

lexer_init(filename)
{
	extrn	open;

	lexer_fd = open(filename, 0);
	return (lexer_fd < 0);
}
