/*
 * Synchronization lab, TASK2.2: waiting for a mutex, or giving up.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

#define SL_STACK_SIZE 1024
#define SL_PRIORITY 5

#define VISITS 3
#define MOTHER_TIME 2000	/* how long the mother holds the account (ms) */
#define DAUGHTER_TIME 500	/* how long the daughter holds the account (ms) */
#define COME_BACK_TIME 700	/* how long to wait before the next visit (ms) */

/* Change K_NO_WAIT to K_FOREVER and compare the output. */
#define LOCK_TIMEOUT K_NO_WAIT

K_MUTEX_DEFINE(account_mutex);

void visit_account(const char *name, int hold_time)
{
	for (int i = 0; i < VISITS; i++) {
		printf("%6lld ms  %s: trying to open the account\n", k_uptime_get(), name);
		if (k_mutex_lock(&account_mutex, LOCK_TIMEOUT) == 0) {
			printf("%6lld ms  %s: the account is mine\n", k_uptime_get(), name);
			/* Too long for a real critical section - it is here to make the waiting visible. */
			k_sleep(K_MSEC(hold_time));
			printf("%6lld ms  %s: done\n", k_uptime_get(), name);
			k_mutex_unlock(&account_mutex);
		} else {
			printf("%6lld ms  %s: the account is busy, I will come back later\n",
			       k_uptime_get(), name);
		}
		k_sleep(K_MSEC(COME_BACK_TIME));
	}
}

void mother(void *p1, void *p2, void *p3)
{
	visit_account("mother  ", MOTHER_TIME);
}

void daughter(void *p1, void *p2, void *p3)
{
	visit_account("daughter", DAUGHTER_TIME);
}

K_THREAD_DEFINE(mother_tid, SL_STACK_SIZE, mother, NULL, NULL, NULL, SL_PRIORITY, 0, 0);
K_THREAD_DEFINE(daughter_tid, SL_STACK_SIZE, daughter, NULL, NULL, NULL, SL_PRIORITY, 0, 100);

int main(void)
{
	k_thread_join(mother_tid, K_FOREVER);
	k_thread_join(daughter_tid, K_FOREVER);
	printf("All visits finished.\n");
	return 0;
}
