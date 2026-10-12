/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

/* Task 1.4 with k_thread_join: main waits for the threads instead of sleeping. */

#define THREAD_STACKSIZE 1024

#define PRODUCER_WORK_MSEC 2000 // Simulated work of the producer before it sends data

#define PIPE_SIZE 100
#define PIPE_MESSAGE "Producer: Hello"

K_THREAD_STACK_DEFINE(producer_stack, THREAD_STACKSIZE);
K_THREAD_STACK_DEFINE(consumer_stack, THREAD_STACKSIZE);

struct k_thread producer_thread;
struct k_thread consumer_thread;

K_PIPE_DEFINE(lab_pipe, PIPE_SIZE, 4);


void producer_thread_fcn(void *p1, void *p2, void *p3) {

	int bytes_written;

	printf("Producer is starting, working for %d ms.\n", PRODUCER_WORK_MSEC);
	k_sleep(K_MSEC(PRODUCER_WORK_MSEC));

	bytes_written = k_pipe_write(&lab_pipe, PIPE_MESSAGE, sizeof(PIPE_MESSAGE), K_NO_WAIT);
	if (bytes_written != sizeof(PIPE_MESSAGE)) {
		printf("Producer: Error writing to the pipe.\n");
		return;
	}
	printf("Producer: %d bytes sent, finishing.\n", bytes_written);
}


void consumer_thread_fcn(void *p1, void *p2, void *p3) {

	char buffer[PIPE_SIZE];
	int bytes_read;

	printf("Consumer is starting.\n");
	bytes_read = k_pipe_read(&lab_pipe, buffer, sizeof(buffer), K_NO_WAIT);
	if (bytes_read <= 0) {
		printf("Consumer: No data in the pipe.\n");
		return;
	}
	printf("Consumer: %d bytes read: %s\n", bytes_read, buffer);
}


int main(void) {
	int64_t start = k_uptime_get();

	printf("Main: Starting producer.\n");
	k_thread_create(&producer_thread, producer_stack, THREAD_STACKSIZE,
			producer_thread_fcn, NULL, NULL, NULL,
			-1, K_INHERIT_PERMS, K_NO_WAIT);

	// Instead of guessing how long to sleep, main is blocked until the producer ends.
	k_thread_join(&producer_thread, K_FOREVER);
	printf("Main: Producer finished after %lld ms, starting consumer.\n",
	       k_uptime_get() - start);

	k_thread_create(&consumer_thread, consumer_stack, THREAD_STACKSIZE,
			consumer_thread_fcn, NULL, NULL, NULL,
			-1, K_INHERIT_PERMS, K_NO_WAIT);

	k_thread_join(&consumer_thread, K_FOREVER);
	printf("Main: Both threads finished after %lld ms.\n", k_uptime_get() - start);

	return 0;
}
