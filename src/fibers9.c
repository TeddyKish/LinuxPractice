/*
 * fibers9.c

 *
 *  Created on: May 25, 2020
 *      Author: Teddy Kishinevsky
 */

#include <stdlib.h>
#include "fibers9.h"

#define KB (1024)
#define MB (1000 * KB)
#define MAX_FIBERS (100)

#define FIBERS9_NONACTIVE_FIBER_ID (-1)
#define FIBERS9_ERROR_VALUE (-1)

typedef struct fiber_s
{
	int id;
	jmp_buf context;
	fiber_main_t func_to_call;
	void* func_argument;
	void* stack;
} fiber_t;

typedef struct fiber_queue_s
{
	int fiber_ids[MAX_FIBERS];
	int head_index;
	int tail_index;
} fiber_queue_t;

fiber_t fibers[MAX_FIBERS];
fiber_queue_t scheduling_queue;
int active_fiber_id;

static fiber_t* get_free_fiber()
{
	for (int i = 0; i < MAX_FIBERS; i++)
	{
		if (fibers[i].id == FIBERS9_NONACTIVE_FIBER_ID)
		{
			return &(fibers[i]);
		}
	}

	return NULL;
}

static int get_free_id()
{
	// No need to check the main fiber ID as it is always taken
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

	// MAX_FIBERS has been reached
	if (free_id == MAX_FIBERS)
	{
		return FIBERS9_ERROR_VALUE;
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

static void reset_fiber(fiber_t* target)
{
	target->id = FIBERS9_NONACTIVE_FIBER_ID;
	target->func_to_call = NULL;
	target->func_argument = NULL;
	target->stack = NULL;
}

static void kill_fiber(fiber_t* dead_fiber)
{
	free(dead_fiber->stack);
	reset_fiber(dead_fiber);
}

static void fibers9_init_queue()
{
	// Initializing the queue
	scheduling_queue.head_index = 0;
	scheduling_queue.tail_index = 0;

	for (int i = 1; i < MAX_FIBERS; i++)
	{
		scheduling_queue.fiber_ids[i] = FIBERS9_NONACTIVE_FIBER_ID;
	}
}

static void fibers9_scheduling_enqueue(int id)
{
	scheduling_queue.fiber_ids[scheduling_queue.tail_index] = id;
	scheduling_queue.tail_index = (scheduling_queue.tail_index + 1) % MAX_FIBERS;
}

static int fibers9_scheduling_dequeue()
{
	int dequeued = scheduling_queue.fiber_ids[scheduling_queue.head_index];
	scheduling_queue.fiber_ids[scheduling_queue.head_index] = FIBERS9_NONACTIVE_FIBER_ID;
	scheduling_queue.head_index = (scheduling_queue.head_index + 1) % MAX_FIBERS;

	return dequeued;
}

void fibers9_init()
{
	// Reserved for further actions, if needed
	for (int i = 0; i < MAX_FIBERS; i++)
	{
		reset_fiber(&(fibers[i]));
	}

	// The caller of fibers9_init is perceived as 'main', so it receives fid 0.
	// Moreover, its function/arg/stack are all NULL, because they are never used.
	fibers[0].id = 0;
	active_fiber_id = fibers[0].id;

	fibers9_init_queue();
}

int fibers9_create(fiber_main_t func_to_call, void* arg)
{
	fiber_t* old_fiber = get_fiber_by_id(active_fiber_id);

	volatile int self_id = active_fiber_id;
	volatile int new_id = get_free_id();
	fiber_t* new_fiber = get_free_fiber();

	if (new_id == FIBERS9_ERROR_VALUE || new_fiber == NULL)
	{
		return 0;
	}

	fibers9_scheduling_enqueue(active_fiber_id);

	new_fiber->id = new_id;
	new_fiber->func_to_call = func_to_call;
	new_fiber->func_argument = arg;

	active_fiber_id = new_fiber->id;

	int setjmp_retval = setjmp(old_fiber->context);

	if (setjmp_retval == 0)
	{
		void* new_stack = malloc(MB);
		new_fiber->stack = new_stack; // Set the stack pointer in the fiber structure before modifying its value

		new_stack += MB; // new_stack points to the end of the allocated memory, because the stack grows down

		asm("movl %0, %%esp;":: "r"(new_stack));

		// Return value can be used if it is needed
		fiber_t* new_current_fiber = get_fiber_by_id(active_fiber_id);
		void* fiber_retval = new_current_fiber->func_to_call(new_current_fiber->func_argument);

		// After the new fiber returned, we switch to the "main" fiber by default
		longjmp((get_fiber_by_id(0))->context, 2);
	}
	else if (setjmp_retval == 1)
	{
		// Return to regular execution
	}
	else if (setjmp_retval == 2)
	{
		// Kill the previous fiber
		kill_fiber(get_fiber_by_id(active_fiber_id));
	}

	active_fiber_id = self_id;

	return new_id;
}

void fibers9_yield()
{
	volatile int self_id = active_fiber_id;
	fiber_t* current_fiber = get_fiber_by_id(self_id); // Should never return NULL

	fibers9_scheduling_enqueue(self_id);

	fiber_t* lru_fiber = get_fiber_by_id(fibers9_scheduling_dequeue());

	int setjmp_retval = setjmp(current_fiber->context);

	if (setjmp_retval == 0)
	{
		longjmp(lru_fiber->context, 1);
	}
	else if (setjmp_retval == 1)
	{
		// Return to regular execution
	}
	else if (setjmp_retval == 2)
	{
		// Kill the previous fiber
		kill_fiber(get_fiber_by_id(active_fiber_id));
	}

	active_fiber_id = self_id;
}

int fibers9_self()
{
	return active_fiber_id;
}

void fibers9_destroy()
{
	// Reserved for further actions, if needed
	for (int i = 1; i < MAX_FIBERS; i++)
	{
		reset_fiber(&(fibers[i]));
	}

	active_fiber_id = FIBERS9_NONACTIVE_FIBER_ID;
}
