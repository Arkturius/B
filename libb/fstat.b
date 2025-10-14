fstat(fd, status)
{
	extrn	syscall;
	auto	st 16, i;

	syscall(108, fd, st);

	status[0] = st[0] & 0xffff;
	status[1] = st[1];
	status[2] = st[2] & 0xffff;
	status[3] = st[2] >> 16;
	status[4] = st[3] & 0xffff;
	status[5] = st[3] >> 16;
	status[6] = st[4] & 0xffff;
	status[7] = st[5];
	status[8] = st[8];
	status[9] = st[10];
	status[10] = st[12];
	
	i = 11;
	while (i < 20)
	{
		status[i] = 0;
		++i;
	}
}
