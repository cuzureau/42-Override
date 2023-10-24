#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <strings.h>
#include <sys/prctl.h>
#include <sys/ptrace.h>

int main(int ac, char **av)
{
	pid_t	pid = fork();
	char	buf[32];

	bzero(buf, 32);
	int		a = 0;
	int		b = 0;
	if (pid == 0)
	{
		prctl(1, 1);
		ptrace(0, 0, 0, 0);
		puts("Give me some shellcode, k");
		gets(buf);

		return 0;
	}
	long ret;
	int tmp;
	int tmp2;
	while (ret = ptrace(3, pid, 44, 0) != 11)
	{
		wait(&b);
		tmp = b;
		tmp2 = b;
		if ((tmp & 127) == 0 || (((tmp2 & 127) + 1) / 2 > 0))
		{
			puts("child is exiting...");
			return 0;
		}
	}
	puts("no exec() for you");
	kill(pid, 9);
	return 0;
}