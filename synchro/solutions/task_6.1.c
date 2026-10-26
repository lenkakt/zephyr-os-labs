/*
 * Synchronization lab, TASK6: sharing data with an interrupt handler.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

#define WITHDRAWALS 100
#define AMOUNT 100
#define PAYMENT_PERIOD K_MSEC(10)

/* 0: no protection, 1: spinlock, 2: mutex (try it with -DCONFIG_ASSERT=y) */
#define PROTECTION 0

long balance = 0;
int payments = 0;

#if PROTECTION == 1
struct k_spinlock account_lock;
#define LOCK_ACCOUNT() k_spinlock_key_t key = k_spin_lock(&account_lock)
#define UNLOCK_ACCOUNT() k_spin_unlock(&account_lock, key)
#elif PROTECTION == 2
K_MUTEX_DEFINE(account_mutex);
#define LOCK_ACCOUNT() k_mutex_lock(&account_mutex, K_FOREVER)
#define UNLOCK_ACCOUNT() k_mutex_unlock(&account_mutex)
#else
#define LOCK_ACCOUNT()
#define UNLOCK_ACCOUNT()
#endif

/* The timer callback runs in an interrupt: the mother's regular payment. */
void payment(struct k_timer *timer)
{
	LOCK_ACCOUNT();
	balance += AMOUNT;
	payments++;
	UNLOCK_ACCOUNT();
}

K_TIMER_DEFINE(payment_timer, payment, NULL);

int main(void)
{
	k_timer_start(&payment_timer, PAYMENT_PERIOD, PAYMENT_PERIOD);

	/* The daughter withdraws money in a thread. */
	for (int i = 0; i < WITHDRAWALS; i++) {
		LOCK_ACCOUNT();
		long current = balance;
		k_busy_wait(2000);	/* the bank checks the withdrawal for 2 ms */
		balance = current - AMOUNT;
		UNLOCK_ACCOUNT();
	}

	k_timer_stop(&payment_timer);
	printf("Payments: %d x %d CZK, withdrawals: %d x %d CZK\n",
	       payments, AMOUNT, WITHDRAWALS, AMOUNT);
	printf("Balance: %ld CZK (expected %ld CZK)\n",
	       balance, (long)(payments - WITHDRAWALS) * AMOUNT);
	return 0;
}
