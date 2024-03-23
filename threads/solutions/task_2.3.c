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

#define USERMODE_ACTIVE

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
	size_t bytes_written;
	int return_value;


	printf("Producer is starting.\n");
	
	//Prepare the message:
	data = (unsigned char *) PIPE_MESSAGE;
	total_size=sizeof(PIPE_MESSAGE);
	
	printf("Producer: Sending %s.\n", data);
	printf("Producer: Sending %d bytes.\n", total_size);

	return_value = k_pipe_put(&lab_pipe, data, total_size, &bytes_written, 1, K_NO_WAIT);
		
	if (return_value != 0) {
		printf("Producer: Error writing to the pipe.\n");
		return;
	}
	else if (bytes_written != total_size) {
		printf("Producer: Not all data sent!\n");
		return;
	}
	else {
		printf("Producer: Data sent successfully.\n");
	}

	return;	
}


void consumer_thread_fcn(void *p1, void *p2, void *p3) {
	#ifdef USERMODE_ACTIVE
		__ASSERT(k_is_user_context(), "User mode is not available on this board!");	
	#endif
	
	unsigned char buffer[100];
	size_t bytes_read;
	int return_value;	

	printf("Consumer is starting.\n");
	return_value = k_pipe_get(&lab_pipe, buffer, sizeof(buffer), &bytes_read, 1, K_NO_WAIT);
	if (return_value != 0) {
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

#ifdef USERMODE_ACTIVE
	//Now grant the permissions to the consumer thread
	//If you do not grant the permsssion, it will not work.
	k_object_access_grant(&lab_pipe, &consumer_thread);
#endif

	printf("Main: Starting consumer\n");

// Here is the difference: K_USER flag is added to the thread creation call:
	k_thread_create(&consumer_thread, consumer_stack, THREAD_STACKSIZE,
			consumer_thread_fcn, NULL, NULL, NULL, -1,
#ifdef USERMODE_ACTIVE			
			K_USER | K_INHERIT_PERMS, 
#else
			K_INHERIT_PERMS, 
#endif
	K_NO_WAIT);
	return 0;
}
