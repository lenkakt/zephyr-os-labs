/*
 * Synchronization lab, TASK5.1: deadlock between two transfers.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

#define SL_STACK_SIZE 1024
#define SL_PRIORITY 5

#define TRANSFERS 1000
#define DEADLOCK_TIMEOUT K_SECONDS(3)

/* Set to 1 to always lock the account with the lower number first. */
#define LOCK_ORDERING 0

struct account {
	int number;
	long balance;
	struct k_mutex lock;
};

struct account mother_account = { .number = 1, .balance = 100000 };
struct account daughter_account = { .number = 2, .balance = 100000 };

void transfer(struct account *from, struct account *to, long amount)
{
	struct account *first = from;
	struct account *second = to;

#if LOCK_ORDERING
	if (from->number > to->number) {
		first = to;
		second = from;
	}
#endif

	k_mutex_lock(&first->lock, K_FOREVER);
	k_yield();			/* the other thread may run here */
	k_mutex_lock(&second->lock, K_FOREVER);

	from->balance -= amount;
	to->balance += amount;

	k_mutex_unlock(&second->lock);
	k_mutex_unlock(&first->lock);
}

void mother(void *p1, void *p2, void *p3)
{
	for (int i = 0; i < TRANSFERS; i++) {
		transfer(&mother_account, &daughter_account, 1);
	}
}

void daughter(void *p1, void *p2, void *p3)
{
	for (int i = 0; i < TRANSFERS; i++) {
		transfer(&daughter_account, &mother_account, 1);
	}
}

/* The threads start 100 ms later, after main has initialized the locks. */
K_THREAD_DEFINE(mother_tid, SL_STACK_SIZE, mother, NULL, NULL, NULL, SL_PRIORITY, 0, 100);
K_THREAD_DEFINE(daughter_tid, SL_STACK_SIZE, daughter, NULL, NULL, NULL, SL_PRIORITY, 0, 100);

int main(void)
{
	k_mutex_init(&mother_account.lock);
	k_mutex_init(&daughter_account.lock);

	if (k_thread_join(mother_tid, DEADLOCK_TIMEOUT) != 0 ||
	    k_thread_join(daughter_tid, DEADLOCK_TIMEOUT) != 0) {
		printf("The transfers did not finish in time - deadlock?\n");
		return 0;
	}
	printf("Mother: %ld CZK, daughter: %ld CZK\n",
	       mother_account.balance, daughter_account.balance);
	return 0;
}
