chdir(path)
{
	extrn	syscall;

	return (syscall(0x0c, path));
}
