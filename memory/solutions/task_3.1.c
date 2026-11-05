/*
 * Memory lab, TASK3.1: measuring how much stack a thread uses.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

#define STACK_SIZE 1024
#define PRIORITY 5

K_THREAD_STACK_DEFINE(quiet_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(talking_stack, STACK_SIZE);
struct k_thread quiet_thread, talking_thread;

void quiet(void *p1, void *p2, void *p3)
{
}

void talking(void *p1, void *p2, void *p3)
{
	printf("Talking thread: %d apples and %d pears\n", 3, 5);
}

static void report(const char *name, struct k_thread *thread)
{
	size_t unused = 0;

	k_thread_stack_space_get(thread, &unused);
	printf("%-8s used %4u of %d bytes\n", name, (unsigned int)(STACK_SIZE - unused), STACK_SIZE);
}

int main(void)
{
	k_thread_create(&quiet_thread, quiet_stack, STACK_SIZE, quiet, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
	k_thread_create(&talking_thread, talking_stack, STACK_SIZE, talking, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
	k_thread_join(&quiet_thread, K_FOREVER);
	k_thread_join(&talking_thread, K_FOREVER);
	report("quiet", &quiet_thread);
	report("talking", &talking_thread);
	return 0;
}
