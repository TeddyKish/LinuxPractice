/*
 * fibers9.c

 *
 *  Created on: May 25, 2020
 *  Author: Teddy Kishinevsky
 */

#include <setjmp.h>
#include <stdlib.h>

#include "fibers9.h"

#define FIBERS9_KILOBYTE (1024)
#define FIBERS9_MEGABYTE (1000 * FIBERS9_KILOBYTE)
#define FIBERS9_MAX_FIBERS (100)

#define FIBERS9_FIBER_YIELDED (1)
#define FIBERS9_FIBER_FINISHED (2)

#define FIBERS9_MAIN_FIBER_ID (0)
#define FIBERS9_FREE_FIBER_ID (-1)

typedef struct fiber_s
{
	int id;
	jmp_buf context;
	fiber_main_t func_to_call;
	void* func_argument;
	void* stack;
} fiber_t;

typedef struct scheduling_queue_s
{
	int fiber_ids[FIBERS9_MAX_FIBERS];
	int head_index;
	int tail_index;
} scheduling_queue_t;

fiber_t fibers[FIBERS9_MAX_FIBERS];
scheduling_queue_t scheduling_queue;
int active_fiber_id;

static fiber_t* _fibers9_get_new_fiber()
{
	for (int i = 0; i < FIBERS9_MAX_FIBERS; i++)
	{
		if (fibers[i].id == FIBERS9_FREE_FIBER_ID)
		{
			return &(fibers[i]);
		}
	}

	return NULL;
}

static int _fibers9_get_new_id()
{
	int free_id = 0;
	int is_free = 1;

	while (is_free)
	{
		is_free = 0;

		for (int i = 0; i < FIBERS9_MAX_FIBERS; i++)
		{
			if (fibers[i].id == free_id)
			{
				is_free = 1;
				free_id++;
				break;
			}
		}
	}

	// FIBERS9_MAX_FIBERS has been reached
	if (free_id == FIBERS9_MAX_FIBERS)
	{
		return FIBERS9_ERROR_VALUE;
	}

	return free_id;
}

static fiber_t* _fibers9_get_existing_fiber(int id)
{
	for (int i = 0; i < FIBERS9_MAX_FIBERS; i++)
	{
		if (fibers[i].id == id)
		{
			return &(fibers[i]);
		}
	}

	return NULL;
}

static void _fibers9_reset_fiber(fiber_t* target)
{
	target->id = FIBERS9_FREE_FIBER_ID;
	target->func_to_call = NULL;
	target->func_argument = NULL;
	target->stack = NULL;
}

static void _fibers9_kill_fiber(fiber_t* dead_fiber)
{
	free(dead_fiber->stack);
	_fibers9_reset_fiber(dead_fiber);
}

static void _fibers9_reset_queue()
{
	// Initializing the queue
	scheduling_queue.head_index = 0;
	scheduling_queue.tail_index = 0;

	for (int i = 0; i < FIBERS9_MAX_FIBERS; i++)
	{
		scheduling_queue.fiber_ids[i] = FIBERS9_FREE_FIBER_ID;
	}
}

static void _fibers9_scheduling_enqueue(int id)
{
	scheduling_queue.fiber_ids[scheduling_queue.tail_index] = id;
	scheduling_queue.tail_index = (scheduling_queue.tail_index + 1) % FIBERS9_MAX_FIBERS;
}

static int _fibers9_scheduling_dequeue()
{
	int dequeued = scheduling_queue.fiber_ids[scheduling_queue.head_index];
	scheduling_queue.fiber_ids[scheduling_queue.head_index] = FIBERS9_FREE_FIBER_ID;
	scheduling_queue.head_index = (scheduling_queue.head_index + 1) % FIBERS9_MAX_FIBERS;

	return dequeued;
}

void fibers9_reset()
{
	// Reserved for further actions, if needed
	for (int i = 0; i < FIBERS9_MAX_FIBERS; i++)
	{
		_fibers9_reset_fiber(&(fibers[i]));
	}

	// The caller of fibers9_init is perceived as 'main', so it receives fid 0.
	// Moreover, its function/arg/stack are all NULL, because they are never used.
	fibers[0].id = FIBERS9_MAIN_FIBER_ID;
	active_fiber_id = fibers[0].id;

	_fibers9_reset_queue();
}

int fibers9_create(fiber_main_t func_to_call, void* arg)
{
	fiber_t* old_fiber = _fibers9_get_existing_fiber(active_fiber_id);
	volatile int self_id = active_fiber_id;

	volatile int new_id = _fibers9_get_new_id();
	fiber_t* new_fiber = _fibers9_get_new_fiber();

	if (new_id == FIBERS9_ERROR_VALUE || new_fiber == NULL)
	{
		return FIBERS9_ERROR_VALUE;
	}

	_fibers9_scheduling_enqueue(active_fiber_id);

	new_fiber->id = new_id;
	new_fiber->func_to_call = func_to_call;
	new_fiber->func_argument = arg;

	active_fiber_id = new_fiber->id;

	int setjmp_retval = setjmp(old_fiber->context);

	if (setjmp_retval == 0)
	{
		void* new_stack = malloc(FIBERS9_MEGABYTE);
		new_fiber->stack = new_stack; // Set the stack pointer in the fiber structure before modifying its value

		new_stack += FIBERS9_MEGABYTE; // new_stack points to the end of the allocated memory, because the stack grows down

		asm("movl %0, %%esp;":: "r"(new_stack));

		// Return value can be used if it is needed
		fiber_t* new_current_fiber = _fibers9_get_existing_fiber(active_fiber_id);
		void* fiber_retval = new_current_fiber->func_to_call(new_current_fiber->func_argument);

		// After the new fiber returned, we switch to the "main" fiber by default
		longjmp((_fibers9_get_existing_fiber(FIBERS9_MAIN_FIBER_ID))->context, FIBERS9_FIBER_FINISHED);
	}
	else if (setjmp_retval == FIBERS9_FIBER_YIELDED)
	{
		// Return to regular execution
	}
	else if (setjmp_retval == FIBERS9_FIBER_FINISHED)
	{
		// Kill the previous fiber
		_fibers9_kill_fiber(_fibers9_get_existing_fiber(active_fiber_id));
	}

	active_fiber_id = self_id;

	return new_id;
}

void fibers9_yield()
{
	volatile int self_id = active_fiber_id;
	fiber_t* current_fiber = _fibers9_get_existing_fiber(active_fiber_id); // Should never return NULL

	_fibers9_scheduling_enqueue(active_fiber_id);
	fiber_t* lru_fiber = _fibers9_get_existing_fiber(_fibers9_scheduling_dequeue());

	int setjmp_retval = setjmp(current_fiber->context);

	if (setjmp_retval == 0)
	{
		longjmp(lru_fiber->context, FIBERS9_FIBER_YIELDED);
	}
	else if (setjmp_retval == FIBERS9_FIBER_YIELDED)
	{
		// Return to regular execution
	}
	else if (setjmp_retval == FIBERS9_FIBER_FINISHED)
	{
		// Kill the previous fiber
		_fibers9_kill_fiber(_fibers9_get_existing_fiber(active_fiber_id));
	}

	active_fiber_id = self_id;
}

int fibers9_self()
{
	return active_fiber_id;
}
