/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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

#define APP_INC_SLEEP 1000
#define APP_DEC_SLEEP 2000


// Define a mutex
K_MUTEX_DEFINE(lab_mutex);



// Global variables
int app_counter = 0;

// First thread: it increases the counter:
void thread_increase() {
	while (1) {
		if (k_mutex_lock(&lab_mutex, K_FOREVER) == 0) {
			app_counter++;
			printf("INC: Counter increased to %d\n", app_counter);
			
			k_mutex_unlock(&lab_mutex);
		}
		k_sleep(K_MSEC(APP_INC_SLEEP));
	}
}

void thread_decrease() {
	while (1) {
		if (k_mutex_lock(&lab_mutex, K_FOREVER) == 0) {
			app_counter--;
			printf("DEC: Counter decreased to %d\n", app_counter);
			k_mutex_unlock(&lab_mutex);
		}
		k_sleep(K_MSEC(APP_DEC_SLEEP));
	}
}



K_THREAD_DEFINE(inc_thread, APP_STACK_SIZE, thread_increase, NULL, NULL, NULL, APP_PRIORITY, 0, 0);
K_THREAD_DEFINE(dec_thread, APP_STACK_SIZE, thread_decrease, NULL, NULL, NULL, APP_PRIORITY, 0, 0);


int main(void)
{
    printf("Booting Zephyr!\n");
    printf("Creating threads and mutexes\n");


    return 0;
}
