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

#define PROCESS_LIST_SIZE (500)
#define BUFFER_SIZE (200)
#define ERROR_VALUE (-1)

typedef struct s_pid_cmd
{
	unsigned int pid;
	unsigned char* comm;
} pid_cmd;

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

void initialize_list(pid_cmd* list, unsigned int size)
{
	for (int i = 0; i < size; i++)
	{
		list[i].pid = -1;
		list[i].comm = NULL;
	}
}

void free_list(pid_cmd* list, unsigned int size)
{
	for (int i = 0; i < size; i++)
	{
		free(list[i].comm);
	}
}

void save_process_list(pid_cmd* list, unsigned int size)
{
	DIR* proc = opendir("/proc");
	struct dirent* curr_entry = readdir(proc);

	char comm_filename[BUFFER_SIZE] = "";

	unsigned int list_pos = 0;

	while (curr_entry != NULL && list_pos < size)
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

			list[list_pos].pid = atoi(curr_entry->d_name);
			list[list_pos].comm = (char*) calloc(BUFFER_SIZE, 1);

			if (read(cmdline_fd, list[list_pos].comm, BUFFER_SIZE - 1) == ERROR_VALUE)
			{
				error_exit("Couldn't read comm content\n");
			}

			reset_buffer(comm_filename, BUFFER_SIZE);
			list_pos++;
		}

		curr_entry = readdir(proc);
	}
}

void print_list(pid_cmd* list, unsigned int size)
{
	for (int i = 0; i < size; i++)
	{
		// We already iterated through all the processes
		if (list[i].pid == -1)
		{
			break;
		}

		printf("%d %s", list[i].pid, list[i].comm);
	}
}

int is_in_list(pid_cmd* list, unsigned int size, unsigned int pid)
{
	for (int i = 0; i < size; i++)
	{
		// We already iterated through all the processes
		if (list[i].pid == -1)
		{
			return 0;
		}

		if (list[i].pid == pid)
		{
			return 1;
		}
	}

	return 0;
}

int main(void)
{
	pid_cmd list[PROCESS_LIST_SIZE];
	pid_cmd second_list[PROCESS_LIST_SIZE];

	initialize_list(list, PROCESS_LIST_SIZE);
	initialize_list(second_list, PROCESS_LIST_SIZE);

	save_process_list(list, PROCESS_LIST_SIZE);
	print_list(list, PROCESS_LIST_SIZE);

	printf("Press any char to scan again\n");
	getchar();

	save_process_list(second_list, PROCESS_LIST_SIZE);

	for (int i = 0; i < PROCESS_LIST_SIZE; i++)
	{
		if (second_list[i].pid == -1)
		{
			break;
		}

		if (!is_in_list(list, PROCESS_LIST_SIZE, second_list[i].pid))
		{
			printf("New Process: %d %s", second_list[i].pid, second_list[i].comm);
		}
	}

	free_list(list, PROCESS_LIST_SIZE);
	free_list(second_list, PROCESS_LIST_SIZE);

	return (0);
}
