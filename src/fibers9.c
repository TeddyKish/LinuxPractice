/*
 * fibers9.c

 *
 *  Created on: May 25, 2020
 *      Author: Teddy Kishinevsky
 */

#include "fibers9.h"

fiber_main_t fibers[MAX_FIBERS];
int active_fiber_id;

void fibers9_init()
{
	// Reserved for further actions, if needed

	fiber_main_t current_fiber;

	for (int i = 0;i < MAX_FIBERS; i++)
	{
		current_fiber = fibers[i];

		current_fiber.id = -1;
		current_fiber.func_to_call = NULL;
		current_fiber.context = NULL;
	}

	// The caller of fibers9_init is perceived as 'main', so it receives fid 0.
	fibers[0].id = 0;
	active_fiber_id = fibers[0].id;
}

int fibers9_create(fiber_main_t fiber_main, void* arg)
{

}

void fibers9_yield()
{

}

int fibers9_self()
{
	return active_fiber_id;
}

void fibers9_destroy()
{
	// Reserved for further actions, if needed

	fiber_main_t current_fiber;

	for (int i = 1;i < MAX_FIBERS; i++)
	{
		current_fiber = fibers[i];

		current_fiber.id = -1;
		current_fiber.func_to_call = NULL;
		current_fiber.context = NULL;
	}

	active_fiber_id = -1;
}
