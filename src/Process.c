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
int signal_sent[30];

void error_exit(const char* msg)
{
	perror(msg);
	exit(-1);
}

void root_signal_handler(int signo)
{
	int cond = 1;

	while (cond)
	{
		cond = 0;

		for (int i = 0; i < 30; i++)
		{
			if (signal_sent[i] == 0)
			{
				// If one of the signals wasn't received, continue iterating the while loop
				cond = 1;
				break;
			}
		}
	}
}

void general_signal_handler(int signo)
{
	if (!(SIGRTMIN <= signo && signo <= SIGRTMAX))
	{
		error_exit("Unsupported signal\n");
	}

	signal_sent[signo - SIGRTMIN - 1] = 1;
}

void recursive_double_fork(unsigned int recursive_level, char* placement, int range_start, int range_end)
{
	int this_rtsig = range_end;
	char* new_placement = (char*) malloc(s_buffer_size);
	printf("%s \n", placement);

	pid_t right_child_pid;
	pid_t left_child_pid;

	if (recursive_level != 0)
	{
		int left_child_range_end = (range_start + range_end - 1) / 2;

		left_child_pid = fork();
		if (left_child_pid == ERROR_VALUE)
		{
			error_exit("Can't fork left child\n");
		}
		else if (left_child_pid == CHILD_PROCESS_FORK_VALUE)
		{
			snprintf(new_placement, s_buffer_size, "%s->Left", placement);
			recursive_double_fork(recursive_level - 1, new_placement, range_start, left_child_range_end);
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
				recursive_double_fork(recursive_level - 1, new_placement, left_child_range_end + 1, range_end - 1);
			}
		}
	}

	// If this is the root process
	if (recursive_level == s_root_height)
	{
		printf("Root waiting for all the signals...\n");

		// Invokes the root signal handler, that verifies that a signal from every child process has been received.
		raise(SIGRTMIN);

		printf("Root received all of the signals \n");
	}
	else
	{
		sleep(5);
		printf("PID( %ld ): waiting to die (SIGNO: %d) \n", (long)getpid(), SIGRTMIN + this_rtsig);
		union sigval empty_union;
		sigqueue(root_pid, SIGRTMIN + this_rtsig, empty_union);
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

	if ((SIGRTMAX - SIGRTMIN) < 30)
	{
		error_exit("Not enough RT-Signals");
	}

	struct sigaction root_sigac;
	root_sigac.sa_flags = SA_RESTART | SA_RESETHAND;
	root_sigac.sa_handler = root_signal_handler;
	const struct sigaction* p_root_sigac = &root_sigac;

	if (sigaction(SIGRTMIN, p_root_sigac, NULL) == ERROR_VALUE)
	{
		error_exit("Can't register ROOT signal handler");
	}

	struct sigaction sigac;
	sigac.sa_flags = SA_RESTART | SA_RESETHAND;
	sigac.sa_handler = general_signal_handler;
	const struct sigaction* p_sigac = &sigac;

	for (int i = SIGRTMIN + 1; i <= SIGRTMIN + 30; i++)
	{
		if (sigaction(i, p_sigac, NULL) == ERROR_VALUE)
		{
			error_exit("Can't catch an RT signal\n");
		}

		signal_sent[i - SIGRTMIN - 1] = 0;
	}

	printf("root pid is %ld\n", (long)getpid());
	recursive_double_fork(s_root_height, "I am Root", 1, 31);
}
