/*
 * Synchronization lab, putting it all together: what does the protection cost?
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

#define OPERATIONS 100000
#define AMOUNT 100

volatile long balance = 0;	/* volatile: the compiler must not merge the additions */
atomic_t atomic_balance = ATOMIC_INIT(0);
K_MUTEX_DEFINE(account_mutex);
struct k_spinlock account_lock;

void report(const char *name, uint32_t start)
{
	uint32_t cycles = k_cycle_get_32() - start;

	printf("%-10s %8u us  %6u ns per operation\n", name,
	       (uint32_t)k_cyc_to_us_floor64(cycles),
	       (uint32_t)(k_cyc_to_ns_floor64(cycles) / OPERATIONS));
}

int main(void)
{
	uint32_t start;

	start = k_cycle_get_32();
	for (int i = 0; i < OPERATIONS; i++) {
		balance += AMOUNT;
	}
	report("none", start);

	start = k_cycle_get_32();
	for (int i = 0; i < OPERATIONS; i++) {
		atomic_add(&atomic_balance, AMOUNT);
	}
	report("atomic", start);

	start = k_cycle_get_32();
	for (int i = 0; i < OPERATIONS; i++) {
		k_spinlock_key_t key = k_spin_lock(&account_lock);

		balance += AMOUNT;
		k_spin_unlock(&account_lock, key);
	}
	report("spinlock", start);

	start = k_cycle_get_32();
	for (int i = 0; i < OPERATIONS; i++) {
		k_mutex_lock(&account_mutex, K_FOREVER);
		balance += AMOUNT;
		k_mutex_unlock(&account_mutex);
	}
	report("mutex", start);

	printf("Cycles per second: %u\n", sys_clock_hw_cycles_per_sec());
	return 0;
}
