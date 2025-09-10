chown(filename, owner)
{
	extern syscall;

	return (syscall(0x5c, filename, owner, owner));
}
