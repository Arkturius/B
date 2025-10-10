open(filename, mode)
{
	extrn	syscall;
	auto	fd;

	fd = syscall(0x05, filename, mode ? 1 : 0);
	return (fd < 0 ? -1 : fd);
}
