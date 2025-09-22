getchar()
{
	auto	c, r;
	extrn	read;

	r = read(0, &c, 1);
	if (r <= 0)
		return (0);
	return (char(c, 0));
}
