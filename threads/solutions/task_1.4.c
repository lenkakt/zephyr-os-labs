/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

/* The two threads are communicating via the pipe.*/


#define THREAD_STACKSIZE 1024

#define MAIN_SLEEP 5000 // Consumer shall start when all data are sent

#define PIPE_SIZE 100
#define PIPE_MESSAGE "Producer: Hello"

K_THREAD_STACK_DEFINE(producer_stack, THREAD_STACKSIZE);
K_THREAD_STACK_DEFINE(consumer_stack, THREAD_STACKSIZE);

//Define threads structures - no difference berween user-space and kernel-space threads yet:
struct k_thread producer_thread;
struct k_thread consumer_thread;

//Define & initialize pipe (called lab_pipe):
K_PIPE_DEFINE(lab_pipe, PIPE_SIZE, 4);



//The producer function: it writes a message to the pipe

void producer_thread_fcn(void *p1, void *p2, void *p3) {

	unsigned char *data;
	size_t total_size;
	int bytes_written;

	printf("Producer is starting.\n");

	//Prepare the message:
	data = (unsigned char *) PIPE_MESSAGE;
	total_size = sizeof(PIPE_MESSAGE);

	printf("Producer: Sending %s.\n", data);
	printf("Producer: Sending %d bytes.\n", total_size);

	bytes_written = k_pipe_write(&lab_pipe, data, total_size, K_NO_WAIT);

	if (bytes_written < 0) {
		printf("Producer: Error writing to the pipe.\n");
		return;
	}
	else if ((size_t) bytes_written != total_size) {
		printf("Producer: Not all data sent!\n");
		return;
	}
	else {
		printf("Producer: Data sent successfully.\n");
	}

	return;
}


void consumer_thread_fcn(void *p1, void *p2, void *p3) {

	unsigned char buffer[100];
	int bytes_read;

	printf("Consumer is starting.\n");
	bytes_read = k_pipe_read(&lab_pipe, buffer, sizeof(buffer), K_NO_WAIT);
	if (bytes_read < 0) {
		printf("Consumer: Error reading from the pipe.\n");
		return;
	}
	else if (bytes_read == 0) {
		printf("Consumer: No data to read.\n");
		return;
	}
	else {
		printf("Consumer: Data read successfully.\n");
	}
	printf("Consumer: %d bytes read.\n", bytes_read);
	for (int i = 0; i < bytes_read; i++) {
		printf("Consumer: %c was in pipe.\n", buffer[i]);
	}
	return;
}


int main(void) {
	printf("Main: Starting threads now.\n");
	printf("Main: Starting producer.\n");

	k_thread_create(&producer_thread, producer_stack, THREAD_STACKSIZE,
			producer_thread_fcn, NULL, NULL, NULL,
			-1, K_INHERIT_PERMS, K_NO_WAIT);

	k_sleep(K_MSEC(MAIN_SLEEP)); //Sleep long enough to allow the producer to finish

	printf("Main: Starting consumer\n");

	k_thread_create(&consumer_thread, consumer_stack, THREAD_STACKSIZE,
			consumer_thread_fcn, NULL, NULL, NULL, -1,
			K_INHERIT_PERMS, K_NO_WAIT);
	return 0;
}
