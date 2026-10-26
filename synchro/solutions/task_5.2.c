/*
 * Synchronization lab, TASK5.2: the dining philosophers.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

#define SL_STACK_SIZE 1024
#define SL_PRIORITY 5

#define PHILOSOPHERS 5
#define MEALS 3
#define DEADLOCK_TIMEOUT K_SECONDS(3)

/* Set to 1 to always take the fork with the lower number first. */
#define FORK_ORDERING 0

struct k_mutex forks[PHILOSOPHERS];

void philosopher(void *p1, void *p2, void *p3)
{
	int id = POINTER_TO_INT(p1);
	int first = id;
	int second = (id + 1) % PHILOSOPHERS;

#if FORK_ORDERING
	if (first > second) {
		first = (id + 1) % PHILOSOPHERS;
		second = id;
	}
#endif

	for (int meal = 1; meal <= MEALS; meal++) {
		k_mutex_lock(&forks[first], K_FOREVER);
		k_msleep(10);		/* thinking about the second fork */
		k_mutex_lock(&forks[second], K_FOREVER);

		printf("%5lld ms  philosopher %d eats meal %d (forks %d and %d)\n",
		       k_uptime_get(), id, meal, first, second);
		k_msleep(10);

		k_mutex_unlock(&forks[second]);
		k_mutex_unlock(&forks[first]);
		k_msleep(10);		/* thinking */
	}
}

static struct k_thread philosopher_threads[PHILOSOPHERS];
K_THREAD_STACK_ARRAY_DEFINE(philosopher_stacks, PHILOSOPHERS, SL_STACK_SIZE);

int main(void)
{
	for (int i = 0; i < PHILOSOPHERS; i++) {
		k_mutex_init(&forks[i]);
	}
	for (int i = 0; i < PHILOSOPHERS; i++) {
		k_thread_create(&philosopher_threads[i], philosopher_stacks[i], SL_STACK_SIZE,
				philosopher, INT_TO_POINTER(i), NULL, NULL,
				SL_PRIORITY, 0, K_NO_WAIT);
	}
	for (int i = 0; i < PHILOSOPHERS; i++) {
		if (k_thread_join(&philosopher_threads[i], DEADLOCK_TIMEOUT) != 0) {
			printf("Philosopher %d is still hungry - deadlock?\n", i);
			return 0;
		}
	}
	printf("All philosophers have eaten.\n");
	return 0;
}
