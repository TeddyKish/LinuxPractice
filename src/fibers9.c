/*
 * fibers9.c

 *
 *  Created on: May 25, 2020
 *      Author: Teddy Kishinevsky
 */

#include "fibers9.h"

typedef struct fiber_s
{
	int id;
	jmp_buf context;
	fiber_main_t func_to_call;
	time_t last_usage;
} fiber_t;

fiber_t fibers[MAX_FIBERS];
int active_fiber_id;

static int get_free_id()
{
	int free_id = 1;
	int is_free = 1;

	while (is_free)
	{
		is_free = 0;

		for (int i = 0; i < MAX_FIBERS; i++)
		{
			if (fibers[i].id == free_id)
			{
				is_free = 1;
				free_id++;
				break;
			}
		}
	}

	return free_id;
}

static fiber_t* get_fiber_by_id(int id)
{
	for (int i = 0; i < MAX_FIBERS; i++)
	{
		if (fibers[i].id == id)
		{
			return &(fibers[i]);
		}
	}

	return NULL;
}

static fiber_t* get_lru_fiber()
{
	fiber_t* lru_fiber = NULL;
	time_t lru_fiber_time = fibers[0].last_usage;

	for (int i = 1; i < MAX_FIBERS; i++)
	{
		if (fibers[i].last_usage < lru_fiber_time)
		{
			lru_fiber = &(fibers[i]);
			lru_fiber_time = fibers[i].last_usage;
		}
	}

	return lru_fiber;
}

void fibers9_init()
{
	// Reserved for further actions, if needed

	fiber_t current_fiber;

	for (int i = 0; i < MAX_FIBERS; i++)
	{
		current_fiber = fibers[i];

		current_fiber.id = -1;
		current_fiber.func_to_call = NULL;
	}

	// The caller of fibers9_init is perceived as 'main', so it receives fid 0.
	fibers[0].id = 0;
	active_fiber_id = fibers[0].id;
}

int fibers9_create(fiber_main_t func_to_call, void* arg)
{
	return 1;
}

void fibers9_yield()
{
	fiber_t* current_fiber = get_fiber_by_id(active_fiber_id); // Should never return NULL

	current_fiber->last_usage = time(NULL);

	fiber_t* lru_fiber = get_lru_fiber();

	if (setjmp(current_fiber->context) == 0)
	{
		longjmp(lru_fiber->context, 0);
	}
}

int fibers9_self()
{
	return active_fiber_id;
}

void fibers9_destroy()
{
	// Reserved for further actions, if needed

	fiber_t current_fiber;

	for (int i = 1;i < MAX_FIBERS; i++)
	{
		current_fiber = fibers[i];

		current_fiber.id = -1;
		current_fiber.func_to_call = NULL;
	}

	active_fiber_id = -1;
}
