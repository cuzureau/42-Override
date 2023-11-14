#include <stdio.h>
#include <string.h>
#include <strings.h>

void			clear_stdin()
{
	char c = 0;

	while (1)
	{
		c = getchar();
		if (c == 0xa || c == 0xff)
			break;
	}
}

unsigned int	get_unum()
{
	unsigned int number = 0;

	fflush(stdout);
	scanf("%u", &number);
	clear_stdin();

	return number;
}

int				read_number(int *tab)
{
	unsigned int index = 0;

	printf(" Index: ");
	index = get_unum();
	printf(" Number at data[%u] is %u\n", index, tab[index]);
	return 0;
}

int				store_number(int *tab)
{
	unsigned int number = 0;
	unsigned int index = 0;

	printf(" Number: ");
	number = get_unum();
	printf(" Index: ");
	index = get_unum();

	if ( index % 3 == 0 || (number >> 24) == 183 )
	{
		puts(" *** ERROR! ***");
		puts("   This index is reserved for wil!");
		puts(" *** ERROR! ***");
		return 1;
	}
	tab[index] = number;
	return 0;
}

int 			main(int ac, char **av, char **env)
{
	int		ret = 0;
	char	buffer[20];
	char	tab[100];

	memset(tab, '\0', 100);

	for (int i = 0; av[i] != '\0'; i++)
		memset(av[i], '\0', strlen(av[i]) - 1);

	for (int i = 0; env[i] != '\0'; i++)
		memset(env[i], '\0', strlen(env[i]) - 1);

	puts("----------------------------------------------------\n"\
    	"\n  Welcome to wil's crappy number storage service!   \n"\
    	"----------------------------------------------------"\
    	"\n Commands:                                     ");
	
	while (1)
	{
		printf("Input command: ");
		ret = 1;
		fgets(buffer, 14, stdin);
		buffer[strlen(buffer) - 2] = 0;
		if (strncmp("store", buffer, 5) == 0)
			ret = store_number(&tab);
		else if (strncmp("read", buffer, 4) == 0)
			ret = read_number(&tab);
		else if (strncmp("quit", buffer, 4) == 0)
			return 0;

		if (ret != 0)
			printf(" Failed to do %s command\n", buffer);
		else
			printf(" Completed %s command successfully\n", buffer);

		bzero(buffer, 20);
	}

	return 0;
}
