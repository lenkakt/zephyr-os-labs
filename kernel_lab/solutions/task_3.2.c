/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

#define KL_TIMER_SEC CONFIG_KL_TIMER_SEC

int kl_calls = 0;

void kl_work_handler(struct k_work *work)
{
	kl_calls++;
	printf("Hallo from kernel timer! I am in the %d. iteration.\n", kl_calls);
}


K_WORK_DEFINE(kl_work, kl_work_handler);

void kl_timer_handler(struct k_timer *dummy)
{
    k_work_submit(&kl_work);
}

K_TIMER_DEFINE(kl_timer, kl_timer_handler, NULL);



int main(void)
{
	printf("The timer application is starting. If you want to stop it, use Ctrl+A+X.\n");
	k_timer_start(&kl_timer, K_SECONDS(KL_TIMER_SEC), K_SECONDS(KL_TIMER_SEC));
	return 0;
}
