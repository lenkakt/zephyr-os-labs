/*
 * Synchronization lab, TASK3.1: bank counters guarded by a counting semaphore.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

#define SL_STACK_SIZE 1024
#define SL_PRIORITY 5

#define CUSTOMERS 8
#define COUNTERS 3
#define VISITS 2

#define SERVICE_TIME 500	/* time at the counter (ms) */
#define AWAY_TIME 1500		/* time between two visits (ms) */
#define ARRIVAL_DELAY 100	/* customers arrive one after another (ms) */

/* A stack (LIFO) of the numbers of free counters. */
struct counter_stack {
	int top;
	int items[COUNTERS];
};

struct counter_stack free_counters = { .top = -1 };

void push_counter(struct counter_stack *stack, int counter)
{
	if (stack->top >= COUNTERS - 1) {
		printf("Error: stack overflow\n");
		return;
	}
	stack->items[++stack->top] = counter;
}

int pop_counter(struct counter_stack *stack)
{
	if (stack->top < 0) {
		printf("Error: stack is empty\n");
		return -1;
	}
	return stack->items[stack->top--];
}

/* The semaphore counts free counters, the mutex protects the stack. */
K_SEM_DEFINE(counters_sem, COUNTERS, COUNTERS);
K_MUTEX_DEFINE(stack_mutex);

void customer(void *p1, void *p2, void *p3)
{
	int id = POINTER_TO_INT(p1);
	int counter;

	for (int i = 0; i < VISITS; i++) {
		printf("%6lld ms  customer %d is waiting for a counter\n", k_uptime_get(), id);
		k_sem_take(&counters_sem, K_FOREVER);

		k_mutex_lock(&stack_mutex, K_FOREVER);
		counter = pop_counter(&free_counters);
		k_mutex_unlock(&stack_mutex);

		printf("%6lld ms  customer %d is served at counter %d (free counters: %d)\n",
		       k_uptime_get(), id, counter, k_sem_count_get(&counters_sem));
		k_sleep(K_MSEC(SERVICE_TIME));

		/* First return the counter, then signal that one is free. */
		k_mutex_lock(&stack_mutex, K_FOREVER);
		push_counter(&free_counters, counter);
		k_mutex_unlock(&stack_mutex);
		printf("%6lld ms  customer %d leaves counter %d\n", k_uptime_get(), id, counter);
		k_sem_give(&counters_sem);

		k_sleep(K_MSEC(AWAY_TIME));
	}
}

static struct k_thread customer_threads[CUSTOMERS];
K_THREAD_STACK_ARRAY_DEFINE(customer_stacks, CUSTOMERS, SL_STACK_SIZE);

int main(void)
{
	for (int i = 1; i <= COUNTERS; i++) {
		push_counter(&free_counters, i);
	}

	for (int i = 0; i < CUSTOMERS; i++) {
		k_thread_create(&customer_threads[i], customer_stacks[i], SL_STACK_SIZE,
				customer, INT_TO_POINTER(i + 1), NULL, NULL,
				SL_PRIORITY, 0, K_MSEC(i * ARRIVAL_DELAY));
	}

	for (int i = 0; i < CUSTOMERS; i++) {
		k_thread_join(&customer_threads[i], K_FOREVER);
	}
	printf("The bank is closed.\n");
	return 0;
}
