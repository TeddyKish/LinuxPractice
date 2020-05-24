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
#include <setjmp.h>

#define ERROR_VALUE (-1)

jmp_buf f1_state;
jmp_buf f2_state;

void f1()
{
	printf("Entering first loop...\n");

	while (1)
	{
		if (setjmp(f1_state) != 0)
		{
			printf("A");
			longjmp(f2_state, 1);
		}
		else
		{
			break;
		}
	}
}

void f2()
{
	printf("Entering second loop...\n");

	while (1)
	{
		if (setjmp(f2_state) != 0)
		{
			printf("B");
			longjmp(f1_state, 1);
		}
		else
		{
			break;
		}
	}
}

void error_exit(const char* msg)
{
	perror(msg);
	exit(ERROR_VALUE);
}

int main(int argc, char* argv[])
{
	// Calling the functions to set their jump points, so that we can call them later
	f1();
	f2();

	longjmp(f1_state, 1);
}
