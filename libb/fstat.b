fstat(file, status)
{
	extrn syscall;
	auto stat_v[35];

	syscall(108, file, stat_v);
}
