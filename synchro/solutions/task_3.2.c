/*
 * Synchronization lab, TASK3.2: a semaphore as a signal.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

#define SL_STACK_SIZE 1024
#define SL_PRIORITY 5

#define PAYMENTS 3
#define AMOUNT 1000
#define PAYMENT_INTERVAL 2000	/* the mother sends money every 2 s */

atomic_t balance = ATOMIC_INIT(0);

/* Starts at 0: the daughter has to wait until the mother gives it. */
K_SEM_DEFINE(money_sent, 0, PAYMENTS);

void mother(void *p1, void *p2, void *p3)
{
	for (int i = 0; i < PAYMENTS; i++) {
		k_sleep(K_MSEC(PAYMENT_INTERVAL));
		atomic_add(&balance, AMOUNT);
		printf("%6lld ms  mother: sent %d CZK\n", k_uptime_get(), AMOUNT);
		k_sem_give(&money_sent);
	}
}

void daughter(void *p1, void *p2, void *p3)
{
	for (int i = 0; i < PAYMENTS; i++) {
		printf("%6lld ms  daughter: waiting for money\n", k_uptime_get());
		k_sem_take(&money_sent, K_FOREVER);
		atomic_sub(&balance, AMOUNT);
		printf("%6lld ms  daughter: withdrew %d CZK, balance %ld CZK\n",
		       k_uptime_get(), AMOUNT, (long)atomic_get(&balance));
	}
}

K_THREAD_DEFINE(mother_tid, SL_STACK_SIZE, mother, NULL, NULL, NULL, SL_PRIORITY, 0, 0);
K_THREAD_DEFINE(daughter_tid, SL_STACK_SIZE, daughter, NULL, NULL, NULL, SL_PRIORITY, 0, 0);

int main(void)
{
	k_thread_join(mother_tid, K_FOREVER);
	k_thread_join(daughter_tid, K_FOREVER);
	printf("Done.\n");
	return 0;
}
