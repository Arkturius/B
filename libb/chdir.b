chdir(filename)
{
	extern syscall;
	
	return (syscall(0x50, filename));
}
