#ifndef FIBERS9_H_
#define FIBERS9_H_

/*
 * fibers9.h
 *
 *  Created on: May 25, 2020
 *      Author: Teddy Kishinevsky
 */

#include <sys/types.h>
#include <setjmp.h>

#define MAX_FIBERS (100)

typedef struct fiber_main_s
{
	int id;
	jmp_buf context;
	void* (*func_to_call) (void*);
} fiber_main_t;

void fibers9_init();
int  fibers9_create(fiber_main_t fiber_main, void* arg);
void fibers9_yield();
int  fibers9_self();
void fibers9_destroy();

#endif /* FIBERS9_H_ */
