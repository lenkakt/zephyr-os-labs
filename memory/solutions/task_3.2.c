/*
 * Memory lab, TASK3.2: a local array on a small stack (mps2/an385).
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

#define STACK_SIZE 1024
#define BUFFER_SIZE 800
#define PRIORITY 5

K_THREAD_STACK_DEFINE(stack, STACK_SIZE);
struct k_thread thread;

void worker(void *p1, void *p2, void *p3)
{
	volatile char buffer[BUFFER_SIZE];

	memset((char *)buffer, 1, BUFFER_SIZE);
	printf("Worker: buffer of %d bytes, first byte %d\n", BUFFER_SIZE, buffer[0]);
}

int main(void)
{
	k_thread_create(&thread, stack, STACK_SIZE, worker, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
	k_thread_join(&thread, K_FOREVER);
	printf("Main: worker finished\n");
	return 0;
}
