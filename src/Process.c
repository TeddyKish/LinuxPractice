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

static const unsigned int s_root_height = 4;

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

	// If this is the root process
	if (recursive_level == s_root_height)
	{
		printf("Root waiting for all the signals...\n");

		//TODO: return to this- is there a better way of implementing it?
		// Currently, it just waits for 30 signals regardless of their senders.
		// Receives signals from all of the descendant processes in the tree
		for (int i = 1; i <= 30; i++)
		{
			pause();
		}
	}

	// currently it exits after the forks
	exit(0);
}

int main(void)
{
	// Ex2.0
	printf("Root PID is: %ld\n", (long)getpid());
	recursive_double_fork(s_root_height);

}
