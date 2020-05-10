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
#define SUCCESS_VALUE (0)
#define ERROR_VALUE (-1)

static const unsigned int s_root_height = 4;
static const unsigned int s_buffer_size = 100;

pid_t root_pid;

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
		printf("---------------------AT signal handling function\n");
	}
	else
	{
		error_exit("Unsupported signal\n");
	}
}

void recursive_double_fork(unsigned int recursive_level, char* placement)
{
	char* new_placement = (char*) malloc(s_buffer_size);
	printf("%s \n", placement);

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
			recursive_double_fork(recursive_level - 1, new_placement);
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
				recursive_double_fork(recursive_level - 1, new_placement);
			}
		}
	}

	// If this is the root process
	if (recursive_level == s_root_height)
	{
		printf("Root waiting for all the signals...\n");

		int received_signal = 0;
		sigset_t signal_set;
		if (sigemptyset(&signal_set) != SUCCESS_VALUE)
		{
			error_exit("Can't initialize signal set");
		}

		if (sigfillset(&signal_set) != SUCCESS_VALUE)
		{
			error_exit("Can't fill signal set");
		}

		// Let all signals be received at once, since multiple instances of the same signal are ignored.
		if ((SIGRTMAX - SIGRTMIN) < 30)
		{
			error_exit("Not enough RT-Signals");
		}

		// Receives signals from all of the descendant processes in the tree
		for (int i = 1; i <= 30; i++)
		{
			if (sigwait(&signal_set, &received_signal) != SUCCESS_VALUE)
			{
				error_exit("Error catching signal by sigwait");
			}
		}

		printf("Root received all of the signals \n");
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
	free(new_placement);
	exit(0);
}

int main(void)
{
	// Ex2.2
	root_pid = getpid();

	struct sigaction sigac;
	sigac.sa_flags = SA_RESTART | SA_SIGINFO;
	sigac.sa_handler = signal_handler;
	const struct sigaction* p_sigac = &sigac;
	if (sigaction(SIGRTMIN, p_sigac, NULL) == ERROR_VALUE)
	{
		error_exit("Can't catch SIGRTMIN\n");
	}

	printf("root pid is %ld\n", (long)getpid());
	recursive_double_fork(s_root_height, "I am Root");
}
