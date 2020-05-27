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
#include "fibers9.h"

void perform_action(char letter)
{
	while (1)
	{
		for (int i = 1; i <= 3; i++)
		{
			printf("%c", letter);
		}

		printf(" (FID: %d)\n", fibers9_self());

		fflush(stdout);

		fibers9_yield();
	}
}

void* thread_function(void* arg)
{
	char letter = *((char *) arg);
	free(arg);

	perform_action(letter);

	return NULL;
}

int main(int argc, char* argv[])
{
	fibers9_reset();

	char letter;

	for (letter = 'Z'; letter > 'A'; letter--)
	{
		char* current_letter = (char *) malloc(sizeof(char));
		*current_letter = letter;

		if (fibers9_create(thread_function, (void *) current_letter) == FIBERS9_ERROR_VALUE)
		{
			perror("fibers9_create failed");
			exit(-1);
		}
	}

	perform_action(letter);

	fibers9_reset();
}
