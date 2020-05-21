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

void* thread_function(void* arg)
{
	char letter = *((char *) arg);
	free(arg);

	while (1)
	{
		printf("%c", letter);
	}
}

int main(int argc, char* argv[])
{
	pthread_t tid;
	char letter;
	int err;

	for (letter = 'A'; letter < 'Z'; letter++)
	{
		char* current_letter = (char *) malloc(sizeof(char));
		*current_letter = letter;

		if ((err = pthread_create(&tid, NULL, thread_function, (void *) current_letter)) != 0)
		{
			error_exit_en(err, "Thread creation failed");
		}
	}

	while (1)
	{
		printf("%c", letter);
	}
}
