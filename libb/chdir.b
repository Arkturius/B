chdir(filename)
{
	extrn syscall;
	
	return (syscall(0x50, filename));
}
