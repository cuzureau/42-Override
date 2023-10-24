#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


void	decrypt(int nb)
{
	char	str[17] = "Q}|u`sfg~sf{}|a3";

	size_t	len = strlen(str);
	int i = 0;

	while (i < len)
	{
		str[i] = str[i] ^ nb;
		i++;
	}
	if (strncmp(str, "Congratulations!", 17) == 0)
	{
		system("/bin/sh");
	} else {
		puts("\nInvalid Password");
	}
	return ;
}

void	test(int pass, int nb)
{
	nb -= pass;

	if (nb < 22) {
		decrypt(nb);
		return ;
	}
	decrypt(rand());
	return ;
}

int 	main(int ac, char **av)
{
	srand(time(0));

	puts("***********************************");
	puts("*\t\tlevel03\t\t**");
	puts("***********************************");
	printf("Password:");
	
	int pass;
	scanf("%d", &pass);
	test(pass, 322424845);

	return 0;
}