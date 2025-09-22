open(filename, mode)
{
	extrn	syscall;
	auto	fd;

	if (mode)
		fd = syscall(0x05, filename, 1);
	else
		fd = syscall(0x05, filename, 0);
	if (fd < 0)
		return (-1); /* ERRNO my beloved */
	return (fd);
}
