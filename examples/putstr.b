/**
* putstr.b
*/

stdout 1;

strlen(s)
{
	auto	i;

	i = 0;
	while (char(s, i))
	{
		++i;
	}
	return (i);
}

putstr(s)
{
	extrn	syscall;

	syscall(4, stdout, s, strlen(s));
}
