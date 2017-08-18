/*
 * thread_wait.c
 *
 *  Created on: 2017. 8. 10.
 *      Author: root
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "thread_wait.h"
#include "alloc.h"
#include "log.h"

static ThreadWait_t *ThreadWaiters;

void thread_wait(void)
{
	ThreadWait_t *idx;

	idx = ThreadWaiters;
	while (idx) {
		int rval;
		echo.d("wait for thread: %s (%ld)", idx->name, idx->pid);
		pthread_join(idx->pid, (void **)&rval);
		echo.d("terminated thread: %s (%ld)", idx->name, idx->pid);
		idx = idx->next;
	}
}

void register_thread_waiter(const char *thread_name, pthread_t pid)
{
	ThreadWait_t *idx;

	idx = ThreadWaiters;
	if (!idx) {
		ThreadWaiters = alloc_sizeof(ThreadWait_t);
		ThreadWaiters->name = strdup(thread_name);
		ThreadWaiters->pid = pid;
	} else {
		while (idx->next) {
			idx = idx->next;
		}

		idx->next = alloc_sizeof(ThreadWait_t);

		idx = idx->next;
		idx->name = strdup(thread_name);
		idx->pid = pid;
	}
	echo.d("register thread wait [%s]", thread_name);
}

void free_thread_wait(ThreadWait_t *mod)
{
	ThreadWait_t *idx;

	if (!mod) {
		mod = ThreadWaiters;
	}
	while (mod) {
		idx = mod->next;
		if (mod->name) free(mod->name);
		free(mod);

		mod = idx;
	}
}
