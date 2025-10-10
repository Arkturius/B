getchar()
{
	auto	c, r;
	extrn	read;

	r = read(0, &c, 1);
	return (r > 0 ? char(c, 0) : 0);
}
