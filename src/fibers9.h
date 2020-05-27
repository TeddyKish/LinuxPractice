#ifndef FIBERS9_H_
#define FIBERS9_H_

/*
 * fibers9.h
 *
 *  Created on: May 25, 2020
 *      Author: Teddy Kishinevsky
 */

#include <setjmp.h>

typedef void* (*fiber_main_t) (void*);

void fibers9_init();
int  fibers9_create(fiber_main_t func_to_call, void* arg);
void fibers9_yield();
int  fibers9_self();
void fibers9_destroy();

#endif /* FIBERS9_H_ */
