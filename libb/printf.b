/*
     NOTE: this is currently a putstr, TODO: make a real printf.
*/

strlen(s)
{
	auto	i;

	i = 0;
	while (char(s, i)) i++;
	return (i);
}

printf(s)
{
	extrn	syscall;

	return(syscall(4, 1, s, strlen(s)));
}
