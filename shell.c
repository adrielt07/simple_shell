#include "header.h"

/**
 * type_prompt - displays a prompt asking the user to input a command
 */
void type_prompt(void)
{
	char *s = "$ ";

	write(STDOUT_FILENO, s, 2);
}

/**
 * read_cmd - reads user input for command to execute, along with parameters
 * @cmd: text string of command name
 * @param: array of text strings of parameters
 */
void read_cmd(char **param)
{
	char *lineptr, **tok, *tmp, *token;
	ssize_t gline;
	size_t n = 0;
	int i = 0, j = 0;

	printf("Entered read_cmd\n");
	tok = malloc(128 * sizeof(char *));
	if (tok == NULL)
	{
		exit(EXIT_FAILURE);
	}
	tmp = malloc(128 * sizeof(char *));
	if (tmp == NULL)
	{
		free(tok);
		exit(EXIT_FAILURE);
	}
	printf("passed mallocs. now getlining...\n");
	gline = getline(&lineptr, &n, stdin);
	if (gline == -1)
	{
		free(tok);
		free(lineptr);
		exit(EXIT_FAILURE);
	}
	printf("gline = %li\n", gline);
	while (lineptr[j] != '\n')
	{
		tmp[j] = lineptr[j];
		j++;
	}
	printf("setting null term at end of cut line\n");
	tmp[j] = '\0';

	printf("tokenizing...\n");
	token = strtok(tmp, " \n");
	printf("first token is: %s\n", token);
	while (token != NULL)
	{
		tok[i] = token;
		i++;
		token = strtok(NULL, " \n");
	}
	printf("tokenizing complete. now MAGIC\n");
	/* THE MAGIC IS HERE: */
//	printf("pre-cpy [cmd = %s] and [tok[0] = %s]\n", cmd, tok[0]);
//	strcpy(cmd, tok[0]); /* set command and parameters from input tokens */
//	printf("post-cpy [cmd = %s] and [tok[0] = %s]\n", cmd, tok[0]);
	printf("assigning params...\n");
	for (j = 0; j < i; j++)
		param[j] = tok[j];
	printf("params assigned.\n");
	param[j] = NULL;

	free(tok);
	free(tmp);
	free(lineptr);
}

/**
 * find_cmd - searches the systems PATHs for a command
 * @tcmd: a command as typed into the shell
 *
 * Return: a string on success, the directory the command is in; 0 if not found.
 */
char *find_cmd(char *tcmd)
{
	int i = 0;
	char *path, *paths[256], *location;

	location = malloc(200 * sizeof(char));
	if (location == NULL)
		return (NULL);

	/* finds environment variable PATHs in system */
	while (environ[i])
	{
		if (strncmp(environ[i], "PATH=", 5) == 0)
		{
			path = (environ[i] + 5);
			break;
		}
		i++;
	}
	i = 0;
	/* parses paths into seperate tokens */
	paths[0] = strtok(path, ":");
	while (paths[i] != NULL)
	{
		i++;
		paths[i] = strtok(NULL, ":");
	}

	i = 0;
	while(paths[i])
	{
		strcpy(location, paths[i]); /* Copying Path[i] to *location */
		strcat(location, "/");      /* add "/" at the end */
		strcat(location, tcmd);    /* add tcmd to make full path */
		if (location_check(location) == 0)
			return (location);
		i++;
	}
	printf("command or directory not found\n");

	free(location);
	return (NULL);
}

/**
 * location_check - looks if cmd exists in current directory
 * @cmd: takes command
 *
 * Return: 0 if found, otherwise 1
 */
int location_check(char *cmd)
{
	struct stat st;

	return(stat(cmd, &st));
}

/**
 * main - looping shell function, executing command if correctly entered
 *
 * Return: -1 on error, otherwise nothing (gives control to executed command)
 */
int main(void)
{
	pid_t hmm;
	char cmd[100], tcmd[100], *param[100], *not_found;

	not_found = "command or directory not found\n";
	while (1)
	{
		type_prompt();
		read_cmd(param);
		if (strcmp(param[0], "\n") != 0)
		{
			//	printf("read successful!\n");
			if (strcmp (param[0], "exit") == 0)
				return (0);

			printf("Forking\n");
			if (fork() != 0)
			{
				hmm = wait(NULL);
				if (hmm == -1)
					return (-1);
			}
			else
			{
				/* CHANGE ME TO _STRCPY etc. */
				if (param[0][0] == '\0')
					break;
				else if(param[0][0] == '.' && param[0][1] == '/')
					if (location_check(param[0]) == 0)
						strcpy(cmd, param[0]);
					else
					{
						printf("%s", not_found);
						return(-1);
					}
				else if (param[0][0] != '/')
					strcpy(cmd, find_cmd(param[0]));
				else
				{
					if (location_check(param[0]) == 0)
						strcpy(cmd, param[0]);
					else
					{
						printf("%s", not_found);
						return(-1);
					}
				}
				execve(cmd, param, NULL);
			}
		}
	}
	return (0);
}
