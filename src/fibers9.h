/*
 * fibers9.h
 *
 *  Created on: May 25, 2020
 *  Author: Teddy Kishinevsky
 */

#ifndef FIBERS9_H_
#define FIBERS9_H_

typedef void* (*fiber_main_t) (void*);

#define FIBERS9_ERROR_VALUE (-1)

/**
 * Resets the fibers9 module.
 * This function should always be called first, before using the rest of the functions in this module.
 * Moreover, it can always be called to reset the fibers to the initial state.
 */
void fibers9_reset();

/**
 * Creates a new fiber and calls <func_to_call(arg)>.
 * On error (for example, the maximum amount of fibers has been reached), FIBERS9_ERROR_VALUE is returned.
 */
int  fibers9_create(fiber_main_t func_to_call, void* arg);

/**
 * Yields the execution to the least recently used fiber.
 */
void fibers9_yield();

/**
 * Returns the current fiber's ID.
 */
int  fibers9_self();

#endif /* FIBERS9_H_ */
