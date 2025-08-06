#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>

#define RESOURCE_DIR	"./tests/resources"
#define	OUTPUT_DIR		"./tests/output"

int	run_cmd(const char **cmd)
{
	int	pid = 0;
	int	ret = 0;

	pid = fork();
	if (!pid)
	{
		static char		buff[64] = {0};
	
		sprintf(buff, "%s/%s", OUTPUT_DIR, cmd[1]);
		buff[strlen(buff) - 1] = 's';
		
		int	fd_out = open(buff, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd_out == -1)
			exit(1);
		dup2(fd_out, STDOUT_FILENO);
		close(fd_out);

		sprintf(buff, "%s/%s", RESOURCE_DIR, cmd[1]);
		cmd[1] = buff;
		execve(cmd[0], (char *const *)cmd, NULL);
		exit(1);
	}
	waitpid(pid, &ret, 0);
	return (ret);
}


const char	*next_file(void)
{
	static DIR		*dp = NULL;
	struct dirent	*ep = NULL;

	if (!dp)
		dp = opendir(RESOURCE_DIR);
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

#define	TEST_FMT_OUT	"\033[3m%16s\033[0m | %s\n"
#define	TEST_FMT_KO		"\033[31;1mKO\033[0m"
#define	TEST_FMT_OK		"\033[32;1mOK\033[0m"

int	run_test(const char *file)
{	
	const char	*bcomp[] = {"./B", file, NULL};
		
	int ret = run_cmd(bcomp);
	printf(TEST_FMT_OUT, file, ret ? TEST_FMT_KO : TEST_FMT_OK);

	return (ret);
}

int main(void)
{
	const char	*s;
	int			i = 0;

	while ((s = next_file()))
		run_test(s);
}
