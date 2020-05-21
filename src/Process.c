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
#include <errno.h>
#include <pthread.h>

#define ERROR_VALUE (-1)

void error_exit_en(int en, const char* msg)
{
	errno = en;
	perror(msg);
	exit(ERROR_VALUE);
}

void print_id(const char* msg)
{
	pthread_t tid = pthread_self();
	pid_t pid = getpid();
	printf("%s: PID = %lu, TID = %lu (0x%lx)\n", msg, (unsigned long) pid, (unsigned long) tid, (unsigned long) tid);
}

void* thread_function(void* arg)
{
	print_id(" New Thread");
	return (void *) 0;
}

int main(int argc, char* argv[])
{

	pthread_t tid;
	int err;

	if ((err = pthread_create(&tid, NULL, thread_function, NULL)) != 0)
	{
		error_exit_en(err, "Can't create thread");
	}

	print_id("Main Thread");
	sleep(3);
	exit(0);
}
