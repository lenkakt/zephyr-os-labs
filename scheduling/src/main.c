/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/random/random.h>

/* Thread set parameters: */
#define THREADS_NUMBER 25
#define THREADS_LOW_PRIORITY 0
#define THREADS_HIGH_PRIORITY 10


/* One thread parameters: */
#define THREAD_STACKSIZE 1024
#define THREAD_SLEEP 500

K_THREAD_STACK_ARRAY_DEFINE(sl_stacks, THREADS_NUMBER, THREAD_STACKSIZE);

static struct k_thread sl_threads[THREADS_NUMBER];

void sl_thread_entry_point(void* sl_thread_nr, void* sl_sleep) {
	int* local_thread_nr = (int*)sl_thread_nr;
	k_timeout_t* local_sleep = (k_timeout_t*)sl_sleep;
	while (true) {
		printf("I am the thread nr. %d with priority %d!\n", *local_thread_nr, k_thread_priority_get(k_current_get()));
		k_sleep(*local_sleep);
	}
}


int main(void)
{	
	printf("Main is starting, creating the threads.");

	int sl_threads_priorities[THREADS_NUMBER];
	k_timeout_t sl_threads_sleeps[THREADS_NUMBER];

	//Set the thread priorities:		
	for (int i = 0; i < THREADS_NUMBER; i++) {		
		sl_threads_priorities[i] = THREADS_LOW_PRIORITY + (sys_rand32_get() % (THREADS_HIGH_PRIORITY - THREADS_LOW_PRIORITY + 1));
	}
	//Set the thread sleeps:
	for (int i = 0; i < THREADS_NUMBER; i++) {
		sl_threads_sleeps[i] = K_MSEC(THREAD_SLEEP);
	}

	for (int i=0; i<THREADS_NUMBER; i++) {
		
		int* current_thread_id; k_timeout_t* current_thread_sleep;
		*current_thread_id = i;
		*current_thread_sleep = sl_threads_sleeps[i];
		
		k_tid_t thread_id = k_thread_create(&sl_threads[i], sl_stacks[i], THREAD_STACKSIZE, sl_thread_entry_point,
                                            current_thread_id, current_thread_sleep, NULL, sl_threads_priorities[i], 0, K_NO_WAIT);
        if (!thread_id) {
            printf("Failed to create thread %d\n", i);
            return -1;
		}
	}

 
	return 0;
}
