/*
 * Synchronization lab, TASK2.3: the balance protected by a mutex.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

#define SL_STACK_SIZE 1024
#define SL_PRIORITY 5

#define OPERATIONS 1000
#define AMOUNT 100

/* Set to 0 to let the threads run without being interrupted in the middle. */
#define SIMULATE_INTERRUPT 1

/* The shared account: mother deposits money, daughter withdraws it. */
long balance = 0;

/* Only the thread that holds the mutex may touch the balance. */
K_MUTEX_DEFINE(account_mutex);

void change_balance(long amount)
{
	k_mutex_lock(&account_mutex, K_FOREVER);

	long current = balance;		/* 1. read the balance */

#if SIMULATE_INTERRUPT
	k_yield();			/* the other thread may run here */
#endif

	balance = current + amount;	/* 2. write the new balance */

	k_mutex_unlock(&account_mutex);
}

void mother(void *p1, void *p2, void *p3)
{
	for (int i = 0; i < OPERATIONS; i++) {
		change_balance(AMOUNT);
	}
}

void daughter(void *p1, void *p2, void *p3)
{
	for (int i = 0; i < OPERATIONS; i++) {
		change_balance(-AMOUNT);
	}
}

K_THREAD_DEFINE(mother_tid, SL_STACK_SIZE, mother, NULL, NULL, NULL, SL_PRIORITY, 0, 0);
K_THREAD_DEFINE(daughter_tid, SL_STACK_SIZE, daughter, NULL, NULL, NULL, SL_PRIORITY, 0, 0);

int main(void)
{
	k_thread_join(mother_tid, K_FOREVER);
	k_thread_join(daughter_tid, K_FOREVER);

	printf("Mother deposited %d CZK, daughter withdrew %d CZK.\n",
	       OPERATIONS * AMOUNT, OPERATIONS * AMOUNT);
	printf("Balance: %ld CZK (expected 0 CZK)\n", balance);
	return 0;
}
