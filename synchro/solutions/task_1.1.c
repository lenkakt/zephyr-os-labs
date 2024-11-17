/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

// Define sleep times and other parameters
#define APP_STACK_SIZE 500
#define APP_PRIORITY 5

#define SLEEP_TIME1 2000
#define SLEEP_TIME2 5000
#define MUTEX_WAIT_TIME 500

// Define a mutex
K_MUTEX_DEFINE(lab_mutex);

// Global variables
uint32_t t1_sleep = SLEEP_TIME1;
uint32_t t2_sleep = SLEEP_TIME2;

// A thread function: It checks if the Mutex is OK, and if it is, it prints a message and sleeps for a while.
void lab_thread(void *thread_nr, uint32_t *sleep_time, void *unused)
{
    uint32_t thread_id = (uint32_t)thread_nr;    	

    while (1) {
		printf("This is thread nr. %d, Trying to get the mutex.\n", thread_id);
        if (k_mutex_lock(&lab_mutex, K_FOREVER) == 0) {
            printf("This is thread nr. %d, Mutex is mine now.\n", thread_id);
			k_sleep(K_MSEC(*sleep_time));
            k_mutex_unlock(&lab_mutex);
			printf("This is thread nr. %d, Mutex just released.\n", thread_id);
        } else {
            printf("This is thread nr. %d, Mutex is busy\n", thread_id);
        }        
    }
	k_sleep(K_MSEC(MUTEX_WAIT_TIME));
}


   K_THREAD_DEFINE(lab_thread1, APP_STACK_SIZE, lab_thread, (void *)1, &t1_sleep, NULL, APP_PRIORITY, 0, 0);
   K_THREAD_DEFINE(lab_thread2, APP_STACK_SIZE, lab_thread, (void *)2, &t2_sleep, NULL, APP_PRIORITY, 0, 0);


int main(void)
{
    printf("Booting Zephyr!\n");
    printf("Creating threads and mutexes\n");


    return 0;
}
