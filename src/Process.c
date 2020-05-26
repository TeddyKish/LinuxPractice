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
#include "fibers9.h"

void error_exit(const char* msg)
{
	perror(msg);
	exit(-1);
}

void perform_action(char letter)
{
	while (1)
	{
		for (int i = 1; i <= 3; i++)
		{
			printf("%c", letter);
		}

		printf(" %d\n", fibers9_self());

		fflush(stdout);

		fibers9_yield();
	}
}

void* thread_function(void* arg)
{
	char letter = *((char *) arg);
	free(arg);

	perform_action(letter);
}

int main(int argc, char* argv[])
{
	fibers9_init();

	char letter;

	for (letter = 'A'; letter < 'Z'; letter++)
	{
		char* current_letter = (char *) malloc(sizeof(char));
		*current_letter = letter;

		if (fibers9_create(thread_function, (void *) current_letter) == 0)
		{
			error_exit("Fiber creation failed");
		}
	}

	perform_action(letter);

	fibers9_destroy();
}
