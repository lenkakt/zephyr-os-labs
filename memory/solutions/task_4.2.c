/*
 * Memory lab, TASK4.2: a memory domain gives the user thread access (qemu_cortex_r5).
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/app_memory/app_memdomain.h>
#include <zephyr/sys/libc-hooks.h>

K_APPMEM_PARTITION_DEFINE(shared_part);
K_APP_DMEM(shared_part) int secret = 42;

#define STACK_SIZE 1024
K_THREAD_STACK_DEFINE(stack, STACK_SIZE);
struct k_thread thread;
struct k_mem_domain domain;

void user_thread(void *p1, void *p2, void *p3)
{
	printf("User thread: trying to overwrite the secret\n");
	secret = 0;
	printf("User thread: done\n");
}

int main(void)
{
	struct k_mem_partition *parts[] = { &shared_part, &z_libc_partition };

	secret = 43;
	printf("Kernel thread: secret is %d\n", secret);
	k_mem_domain_init(&domain, ARRAY_SIZE(parts), parts);
	k_thread_create(&thread, stack, STACK_SIZE, user_thread, NULL, NULL, NULL, 5, K_USER, K_FOREVER);
	k_mem_domain_add_thread(&domain, &thread);
	k_thread_start(&thread);
	k_thread_join(&thread, K_FOREVER);
	printf("Kernel thread: secret is now %d\n", secret);
	return 0;
}
