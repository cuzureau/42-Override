#include <stdio.h>
#include <string.h>

int	main(int ac, char **av)
{
	int		i = 0;
	char	buf[100];

	fgets(buf, 100, stdin);
	i = 0;

	while (i < strlen(buf))
	{
		if (buf[i] > 64 && buf[i] <= 90)
			buf[i] = buf[i] ^ 32;
		i++;
	}
	printf(buf);
	exit(0);
}
