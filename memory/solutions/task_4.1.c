/*
 * Memory lab, TASK4.1: a user thread writes to kernel data (qemu_cortex_r5).
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

int secret = 42;

#define STACK_SIZE 1024
K_THREAD_STACK_DEFINE(stack, STACK_SIZE);
struct k_thread thread;

void user_thread(void *p1, void *p2, void *p3)
{
	printf("User thread: trying to overwrite the secret\n");
	secret = 0;
	printf("User thread: done\n");
}

int main(void)
{
	secret = 43;
	printf("Kernel thread: secret is %d\n", secret);
	k_thread_create(&thread, stack, STACK_SIZE, user_thread, NULL, NULL, NULL, 5, K_USER, K_NO_WAIT);
	k_thread_join(&thread, K_FOREVER);
	printf("Kernel thread: secret is still %d\n", secret);
	return 0;
}
