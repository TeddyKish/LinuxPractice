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
#include <signal.h>

#define CHILD_PROCESS_FORK_VALUE (0)
#define ERROR_VALUE (-1)

static const unsigned int s_root_height = 4;
static const unsigned int s_buffer_size = 100;

void error_exit(const char* msg)
{
	perror(msg);
	exit(-1);
}

void signal_handler(int signo)
{
	if (signo == SIGRTMIN)
	{
		// Currently do nothing
	}
	else
	{
		error_exit("Unsupported signal\n");
	}
}

void recursive_double_fork(unsigned int recursive_level, pid_t root_pid, char* placement)
{
	char* new_placement = (char*) malloc(s_buffer_size);
	printf(placement);
	printf("\n");

	pid_t right_child_pid;
	pid_t left_child_pid;

	if (recursive_level != 0)
	{

		left_child_pid = fork();
		if (left_child_pid == ERROR_VALUE)
		{
			error_exit("Can't fork left child\n");
		}
		else if (left_child_pid == CHILD_PROCESS_FORK_VALUE)
		{
			snprintf(new_placement, s_buffer_size, "%s->Left", placement);
			recursive_double_fork(recursive_level - 1, root_pid, new_placement);
		}
		else
		{
			right_child_pid = fork();
			if (right_child_pid == ERROR_VALUE)
			{
				error_exit("Can't fork right child\n");
			}
			else if (right_child_pid == CHILD_PROCESS_FORK_VALUE)
			{
				snprintf(new_placement, s_buffer_size, "%s->Right", placement);
				recursive_double_fork(recursive_level - 1, root_pid, new_placement);
			}
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
	else
	{
		sleep(5);
		printf("PID( %ld ): waiting to die\n", (long)getpid());
		union sigval empty_union;
		sigqueue(root_pid, SIGRTMIN, empty_union);
		pause();
	}

	// currently it exits after the forks
	exit(0);
}

int main(void)
{
	// Ex2.2
	if (signal(SIGRTMIN, signal_handler) == SIG_ERR)
	{
		error_exit("Can't catch SIGRTMIN\n");
	}

	recursive_double_fork(s_root_height, getpid(), "I am Root");
}
