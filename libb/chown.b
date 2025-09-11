chown(filename, owner)
{
	extrn syscall;

	return (syscall(0x5c, filename, owner, owner));
}
