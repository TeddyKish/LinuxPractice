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
#include <dirent.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE (200)
#define ERROR_VALUE (-1)

void error_exit(const char* msg)
{
	perror(msg);
	exit(-1);
}

int is_process_dirname(const char* entryname, unsigned char type)
{
	if (type != DT_DIR)
	{
		return 0;
	}

	while (*entryname != '\0')
	{
		if (!(isdigit(*entryname)))
		{
			return 0;
		}

		entryname++;
	}

	return 1;
}

void reset_buffer(char* buffer, unsigned int size)
{
	for (int i = 0; i < size; i++)
	{
		buffer[i] = '\0';
	}
}

int main(void)
{
	DIR* proc = opendir("/proc");
	struct dirent* curr_entry = readdir(proc);

	char comm_filename[BUFFER_SIZE] = "";
	char comm_content[BUFFER_SIZE] = "";

	while (curr_entry != NULL)
	{
		if (is_process_dirname(curr_entry->d_name, curr_entry->d_type))
		{
			if (snprintf(comm_filename, BUFFER_SIZE, "/proc/%s/comm", curr_entry->d_name) < 0)
			{
				error_exit("snprintf failed \n");
			}

			int cmdline_fd = open(comm_filename, O_RDONLY);

			if (cmdline_fd == ERROR_VALUE)
			{
				error_exit("Cannot open comm file\n");
			}

			if (read(cmdline_fd, comm_content, BUFFER_SIZE - 1) == ERROR_VALUE)
			{
				error_exit("Couldn't read comm content\n");
			}

			printf("%s %s", curr_entry->d_name, comm_content);

			reset_buffer(comm_filename, BUFFER_SIZE);
			reset_buffer(comm_content, BUFFER_SIZE);
		}

		curr_entry = readdir(proc);
	}
}
