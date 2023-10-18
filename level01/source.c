#include <stdio.h>
#include <string.h>

char	*a_user_name;

int	verify_user_name()
{
	puts("verifying username....\n");

	return (strncmp("dat_wil", a_user_name, 7));
}

int	verify_user_pass(char *pass)
{
	return (strncmp("admin", pass, 5));
}

int main(int ac, char **av)
{
	puts("****** ADMIN LOGIN PROMPT ******");
	printf("Enter Username: ");
	fgets(a_user_name, 256, stdin);

	if (verify_user_name())
	{
		puts("nope, incorrect username...\n");
		return 1;
	}
	puts("Enter Password: ");
	
	char	password[16];
	fgets(password, 100, stdin);

	int answer = verify_user_pass(password);
	if (answer == 0 || answer != 0)
	{
		puts("nope, incorrect password...\n");
		return 1;
	}
	return 0;
}
