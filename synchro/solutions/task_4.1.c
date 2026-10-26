/*
 * Synchronization lab, TASK4: priority inversion.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

#define SL_STACK_SIZE 1024

/* Lower number = higher priority. */
#define PAYMENT_PRIORITY 1
#define BACKUP_PRIORITY 2
#define STATEMENT_PRIORITY 3

/* Set to 1 to protect the account with a mutex instead of a semaphore. */
#define USE_MUTEX 0

#if USE_MUTEX
K_MUTEX_DEFINE(account_mutex);
#define LOCK_ACCOUNT() k_mutex_lock(&account_mutex, K_FOREVER)
#define UNLOCK_ACCOUNT() k_mutex_unlock(&account_mutex)
#define LOCK_KIND "mutex"
#else
K_SEM_DEFINE(account_sem, 1, 1);
#define LOCK_ACCOUNT() k_sem_take(&account_sem, K_FOREVER)
#define UNLOCK_ACCOUNT() k_sem_give(&account_sem)
#define LOCK_KIND "semaphore"
#endif

/* Low priority: the monthly statement reads the account for 100 ms. */
void statement(void *p1, void *p2, void *p3)
{
	LOCK_ACCOUNT();
	printf("%4lld ms  statement: locked the account\n", k_uptime_get());
	k_busy_wait(100 * 1000);
	printf("%4lld ms  statement: unlocked the account\n", k_uptime_get());
	UNLOCK_ACCOUNT();
}

/* Medium priority: a backup that needs the processor, not the account. */
void backup(void *p1, void *p2, void *p3)
{
	k_msleep(40);
	printf("%4lld ms  backup: started\n", k_uptime_get());
	k_busy_wait(500 * 1000);
	printf("%4lld ms  backup: finished\n", k_uptime_get());
}

/* High priority: a card payment has to be fast. */
void payment(void *p1, void *p2, void *p3)
{
	k_msleep(30);
	int64_t start = k_uptime_get();

	printf("%4lld ms  payment: needs the account\n", start);
	LOCK_ACCOUNT();
	printf("%4lld ms  payment: done, waited %lld ms\n",
	       k_uptime_get(), k_uptime_get() - start);
	UNLOCK_ACCOUNT();
}

K_THREAD_DEFINE(statement_tid, SL_STACK_SIZE, statement, NULL, NULL, NULL,
		STATEMENT_PRIORITY, 0, 0);
K_THREAD_DEFINE(backup_tid, SL_STACK_SIZE, backup, NULL, NULL, NULL,
		BACKUP_PRIORITY, 0, 0);
K_THREAD_DEFINE(payment_tid, SL_STACK_SIZE, payment, NULL, NULL, NULL,
		PAYMENT_PRIORITY, 0, 0);

int main(void)
{
	printf("The account is protected by a %s.\n", LOCK_KIND);
	k_thread_join(statement_tid, K_FOREVER);
	k_thread_join(backup_tid, K_FOREVER);
	k_thread_join(payment_tid, K_FOREVER);
	return 0;
}
