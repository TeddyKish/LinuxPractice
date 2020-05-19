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
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ERROR_VALUE (-1)

void error_exit(const char* msg)
{
	perror(msg);
	exit(ERROR_VALUE);
}

void reverse_string(char* string, size_t size)
{
	char tmp;

	for (size_t i = 0; i < (size / 2); i++)
	{
		tmp = string[i];
		string[i] = string[size - i - 1];
		string[size - i - 1] = tmp;
	}
}

int main(int argc, char* argv[])
{
	int fd = -1;
	struct stat file_stat;

	if (argc != 2)
	{
		printf("Usage: %s <filename>\n", argv[0]);
		exit(-1);
	}

	if ((fd = open(argv[1], O_RDWR)) == ERROR_VALUE)
	{
		error_exit("Can't open file");
	}

	if (fstat(fd, &file_stat) == ERROR_VALUE)
	{
		error_exit("Can't get file info");
	}

	void * shared_space = mmap(NULL, file_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (shared_space == MAP_FAILED)
	{
		error_exit("Can't map shared memory");
	}

	close(fd);

	reverse_string((char *) shared_space, file_stat.st_size);

	printf("The file was reversed! \n");
	exit(0);
}
