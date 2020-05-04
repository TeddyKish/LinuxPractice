/*
 ============================================================================
 Name        : Process.c
 Author      : TeddyKish
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void error_exit(void)
{
	printf("ERROR");
	exit(-1);
}

void recursive_double_fork(unsigned int recursive_level)
{
	if (recursive_level == 0)
	{
		return;
	}

	pid_t pid = fork();
	if (pid == -1)
	{
		error_exit();
	}
	else if (pid == 0)
	{
		recursive_double_fork(recursive_level - 1);
	}
	else if (pid != 0)
	{
		pid = fork();
		if (pid == -1)
		{
			error_exit();
		}
		else if (pid == 0)
		{
			recursive_double_fork(recursive_level - 1);
		}
	}

	printf("my pid is %ld \n", (long) getpid());

	// currently it exits after the forks
	exit(0);
}

int main(void)
{
	// Ex2.0
	recursive_double_fork(4);

}
