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
#include <sys/mman.h>

#define ERROR_VALUE (-1)

#define MAX_READ_SIZE (200)

void handle_user_signal(int signo)
{
	if (signo != SIGUSR1)
	{
		error_exit("Received a signal different than SIGUSR1");
	}
}

void error_exit(const char* msg)
{
	perror(msg);
	exit(ERROR_VALUE);
}

void reverse_string(char* string)
{
	size_t size = strnlen(string, MAX_READ_SIZE - 1);
	char tmp;

	for (size_t i = 0; i < (size / 2); i++)
	{
		tmp = string[i];
		string[i] = string[size - i - 1];
		string[size - i - 1] = tmp;
	}
}

int main(void)
{
	void * shared_space = mmap(0, MAX_READ_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	if (shared_space == MAP_FAILED)
	{
		error_exit("Can't map shared memory");
	}

	if (signal(SIGUSR1, handle_user_signal) == SIG_ERR)
	{
		error_exit("Can't handle SIGUSR1");
	}

	pid_t pid;

	if ((pid = fork()) < 0)
	{
		error_exit("Can't fork child process");
	}
	else if (pid > 0) // Parent
	{
		printf("Enter a string to reverse:\n");

		if (fgets((char *) shared_space, MAX_READ_SIZE, stdin) == NULL)
		{
			error_exit("Can't read user input");
		}

		// Notify child that the string has been read
		kill(pid, SIGUSR1);

		pause();

		printf("Reversed: %s\n", shared_space);
	}
	else // Child
	{
		// Wait for parent to read the user input to the shared space
		pause();

		reverse_string((char *) shared_space);

		// Notify parent that the string has been reversed
		kill((long) getppid(), SIGUSR1);
	}

	exit(0);
}
