/*
 * Synchronization lab, TASK3.3: waiting for enough money with a condition variable.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

#define SL_STACK_SIZE 1024
#define SL_PRIORITY 5

#define PAYMENTS 5
#define AMOUNT 100
#define NEEDED 400		/* the daughter needs 400 CZK at once */
#define PAYMENT_INTERVAL 1000

long balance = 0;

K_MUTEX_DEFINE(account_mutex);
K_CONDVAR_DEFINE(balance_changed);

void mother(void *p1, void *p2, void *p3)
{
	for (int i = 0; i < PAYMENTS; i++) {
		k_sleep(K_MSEC(PAYMENT_INTERVAL));
		k_mutex_lock(&account_mutex, K_FOREVER);
		balance += AMOUNT;
		printf("%6lld ms  mother: sent %d CZK, balance %ld CZK\n",
		       k_uptime_get(), AMOUNT, balance);
		k_condvar_signal(&balance_changed);
		k_mutex_unlock(&account_mutex);
	}
}

void daughter(void *p1, void *p2, void *p3)
{
	k_mutex_lock(&account_mutex, K_FOREVER);
	/* while, not if: after waking up, the condition has to be checked again */
	while (balance < NEEDED) {
		printf("%6lld ms  daughter: balance %ld CZK is not enough, waiting\n",
		       k_uptime_get(), balance);
		k_condvar_wait(&balance_changed, &account_mutex, K_FOREVER);
	}
	balance -= NEEDED;
	printf("%6lld ms  daughter: withdrew %d CZK, balance %ld CZK\n",
	       k_uptime_get(), NEEDED, balance);
	k_mutex_unlock(&account_mutex);
}

K_THREAD_DEFINE(mother_tid, SL_STACK_SIZE, mother, NULL, NULL, NULL, SL_PRIORITY, 0, 0);
K_THREAD_DEFINE(daughter_tid, SL_STACK_SIZE, daughter, NULL, NULL, NULL, SL_PRIORITY, 0, 0);

int main(void)
{
	k_thread_join(mother_tid, K_FOREVER);
	k_thread_join(daughter_tid, K_FOREVER);
	printf("Done, balance %ld CZK.\n", balance);
	return 0;
}
