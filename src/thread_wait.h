/*
 * thread_wait.h
 *
 *  Created on: 2017. 8. 10.
 *      Author: root
 */

#ifndef SRC_THREAD_WAIT_H_
#define SRC_THREAD_WAIT_H_

#include <pthread.h>

typedef struct ThreadWait_t {
	char *name;
	pthread_t pid;
	struct ThreadWait_t *next;
} ThreadWait_t;

void thread_wait(void);
void register_thread_waiter(const char *, pthread_t);
void free_thread_wait(ThreadWait_t *mod);

#endif /* SRC_THREAD_WAIT_H_ */
