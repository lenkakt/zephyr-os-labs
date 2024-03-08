/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

/* TL stands for Threads Lab */

#define TL_STACK_SIZE 500
#define TL_PRIORITY 5

#define TL1_SLEEP_MSEC 500
#define TL2_SLEEP_MSEC 777

void tl1_entry_point() {
	while (true) {
		printf("Hello world from my first thread!\n");
		k_sleep(K_MSEC(TL1_SLEEP_MSEC));
	}
}

void tl2_entry_point() {
	while (true) {
		printf("I am the second thread!\n");
		k_sleep(K_MSEC(TL2_SLEEP_MSEC));
	}
}

K_THREAD_DEFINE(tl1_tid, TL_STACK_SIZE, tl1_entry_point, NULL, NULL, NULL, TL_PRIORITY, 0, 0);
K_THREAD_DEFINE(tl2_tid, TL_STACK_SIZE, tl2_entry_point, NULL, NULL, NULL, TL_PRIORITY, 0, 0);


