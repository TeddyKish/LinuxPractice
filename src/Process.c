/*
 ============================================================================
 Name        : Process.c
 Author      : TeddyKish
 Version     :
 Copyright   : Your copyright notice
 Description : Linux exercises
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>

#define ERROR_VALUE (-1)
#define STOP_CHARACTER ('=')

#define PIPE_READ_INDEX (0)
#define PIPE_WRITE_INDEX (1)

void error_exit(const char* msg)
{
	perror(msg);
	exit(ERROR_VALUE);
}

void transform_character(char* c)
{
	if ('A' <= *c && *c <= 'Z')
	{
		*c = tolower(*c);
	}
	else if ('a' <= *c && *c <= 'z')
	{
		*c = toupper(*c);
	}
}

int main(void)
{
	int parent_read_pipe[2];
	int child_read_pipe[2];

	pid_t pid;
	char current_char = '\0';

	if (pipe(parent_read_pipe) < 0 || pipe(child_read_pipe) < 0)
	{
		error_exit("Can't create pipes");
	}

	if ((pid = fork()) < 0)
	{
		error_exit("Failed to fork a child");
	}
	else if (pid > 0) // Parent
	{
		close(child_read_pipe[PIPE_READ_INDEX]);
		close(parent_read_pipe[PIPE_WRITE_INDEX]);

		printf("Enter characters to transform: \n");

		while (1)
		{
			while ((current_char = getchar()) == '\n');

			if (current_char == STOP_CHARACTER)
			{
				break;
			}

			if (write(child_read_pipe[PIPE_WRITE_INDEX], &current_char, 1) != 1)
			{
				error_exit("Can't write to child read pipe");
			}

			if (read(parent_read_pipe[PIPE_READ_INDEX], &current_char, 1)  != 1)
			{
				error_exit("Can't read from parent read pipe");
			}

			printf("Transformed to -> %c\n", current_char);
		}

		kill(pid, SIGTERM);
	}
	else // Child
	{
		close(parent_read_pipe[PIPE_READ_INDEX]);
		close(child_read_pipe[PIPE_WRITE_INDEX]);

		while (1)
		{
			if (read(child_read_pipe[PIPE_READ_INDEX], &current_char, 1) != 1)
			{
				error_exit("Can't read from child read pipe");
			}

			transform_character(&current_char);

			if (write(parent_read_pipe[PIPE_WRITE_INDEX], &current_char, 1) != 1)
			{
				error_exit("Can't write to parent read pipe");
			}
		}
	}

	return 0;
}
