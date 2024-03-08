/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>


/* TL stands for Threads Lab */

#define TL_STACK_SIZE 500
#define TL_PRIORITY 5

#define TL1_SLEEP_MSEC 500
#define TL2_SLEEP_MSEC 777

#define TL1_MESSAGE "1: I am the first thread!\n"
#define TL2_MESSAGE "2: I am the second thread!\n"



//void tl_entry_point(k_timeout_t* tl_sleep, char *tl_message) {chodi
void tl_entry_point(k_timeout_t* tl_sleep, const char tl_message[]) {
	
	while (true) {
		printf("%s\n", tl_message);
		k_sleep(*tl_sleep);
	}
}

	const char tl1_message[50] = TL1_MESSAGE;
	const char tl2_message[50] = TL2_MESSAGE;

	k_timeout_t tl1_sleep = K_MSEC(TL1_SLEEP_MSEC);
	k_timeout_t tl2_sleep = K_MSEC(TL2_SLEEP_MSEC);


	K_THREAD_DEFINE(tl1_tid, TL_STACK_SIZE, tl_entry_point, &tl1_sleep, tl1_message, NULL, TL_PRIORITY, 0, 0);
	K_THREAD_DEFINE(tl2_tid, TL_STACK_SIZE, tl_entry_point, &tl2_sleep, tl2_message, NULL, TL_PRIORITY, 0, 0);



