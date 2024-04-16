/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

/* This first exmample for userspace just defines one userspace thread. */
/* The only difference is in the k_thread_create at the end. */

#define THREAD_STACKSIZE 1024

//Define stacks for both threads - no difference between user-space and kernel-space threads yet:
K_THREAD_STACK_DEFINE(userspace_stack, THREAD_STACKSIZE);
K_THREAD_STACK_DEFINE(kernelspace_stack, THREAD_STACKSIZE);

//Define threads structures - no difference berween user-space and kernel-space threads yet:
struct k_thread userspace_thread;
struct k_thread kernelspace_thread;

//Define thread functions - still no difference between user-space and kernel-space threads:

void userspace_thread_fcn(void *p1, void *p2, void *p3) {
	printf("Userspace: Hello!\n");
    __ASSERT(k_is_user_context(), "User mode is not available on this board!");
}

void kernelspace_thread_fcn(void *p1, void *p2, void *p3) {
	printf("Kernelspace: Hello!\n");
}


int main(void) {
	printf("Main: Starting threads now.\n");
	printf("Main: Creating kernel-space thread\n");

	k_thread_create(&kernelspace_thread, kernelspace_stack, THREAD_STACKSIZE,
			kernelspace_thread_fcn, NULL, NULL, NULL,
			-1, K_INHERIT_PERMS, K_NO_WAIT);

	k_sleep(K_MSEC(1000));

	printf("Main: Creating user-space thread\n");

// Here is the difference: K_USER flag is added to the thread creation call:
	k_thread_create(&userspace_thread, userspace_stack, THREAD_STACKSIZE,
			userspace_thread_fcn, NULL, NULL, NULL,
			-1, K_USER | K_INHERIT_PERMS, K_NO_WAIT);
	return 0;
}
