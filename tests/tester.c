#include <dirent.h>
#include <stdio.h>

const char	*next_file(void)
{
	static DIR		*dp = NULL;
	struct dirent	*ep = NULL;

	if (!dp)
		dp = opendir ("./tests/resources");
	if (dp)
	{
		do {
			ep = readdir(dp);
		} while (ep && ep->d_type != DT_REG);
		if (!ep)
		{
			closedir(dp);
			dp = NULL;
			return (NULL);
		}
		return (ep->d_name);
	}
	return (NULL);
}

int main(void)
{
	const char *s;

	while ((s = next_file()))
		printf("file: %s\n", s);
}
