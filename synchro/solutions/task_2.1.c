/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>


#define APP_STACK_SIZE 500
#define APP_PRIORITY 5

#define NR_OF_CARS 10
#define NR_OF_SLOTS 3

#define APP_THREAD_START_DELAY 2000
#define CAR_PARK_TIME 500
#define CAR_DRIVE_TIME 2000

// A FIFO Stack for the free spots:
typedef struct {
    int top;
    int items[NR_OF_SLOTS];
} app_stack;

app_stack free_slot_stack = { .top = -1 };

// Function to push an element onto the stack
void app_push(app_stack *stack, int value) {
    if (stack->top > NR_OF_SLOTS-1) {
        printf("Error: Stack overflow\n");		
    } else {
        stack->top++;
        stack->items[stack->top] = value;
    }
}

// Function to pop an element from the stack
int app_pop(app_stack *stack) {
    if (stack->top < 0) {
        printf("Error: Stack is empty.\n");
        return -1; // Sentinel value for empty stack
    } else {
        int value = stack->items[stack->top];
        stack->top--;
        return value;
    }
}

K_MUTEX_DEFINE(free_slot_mutex);
K_SEM_DEFINE(free_slot_sem, NR_OF_SLOTS, NR_OF_SLOTS);

void car_fcn(void* car_nr, void* park_time, void* drive_time) {

	int slot_id;
	int local_car_id = POINTER_TO_INT(car_nr);
	uint32_t local_park_time = POINTER_TO_INT(park_time);
	uint32_t local_drive_time = POINTER_TO_INT(drive_time);

	while (1) {
		// Try to park - acquire a semaphore at first:
		printf("Car %d is looking for a parking slot.\n", local_car_id);
		 if (k_sem_take(&free_slot_sem, K_FOREVER) == 0) {
			// If we got the semaphore, try to get a free slot:
			 if (k_mutex_lock(&free_slot_mutex, K_FOREVER) == 0) {
				slot_id = app_pop(&free_slot_stack);				
				k_mutex_unlock(&free_slot_mutex);
				printf("Car %d parked in slot %d\n", local_car_id, slot_id);
				printf("We still have %d free slots.\n", k_sem_count_get(&free_slot_sem));
				// Sleep for the parking time:				
				k_sleep(K_MSEC(local_park_time));
				// Leave the slot:
				if (k_mutex_lock(&free_slot_mutex, K_FOREVER) == 0) {
					app_push(&free_slot_stack, slot_id);
					k_mutex_unlock(&free_slot_mutex);
					k_sem_give(&free_slot_sem);
					printf("Car %d left slot %d\n", local_car_id, slot_id);
				}
			} 			
		}
		// Drive around 
		k_sleep(K_MSEC(local_drive_time)); 
		
	}
}

k_tid_t car_thread_ids[NR_OF_CARS];

static struct k_thread car_threads[NR_OF_CARS];
K_THREAD_STACK_ARRAY_DEFINE(car_stacks, NR_OF_CARS, APP_STACK_SIZE);

void init_cars() {
	// Initialize the free slot stack - noone is parking at the beginning:
	for (int i = 0; i < NR_OF_SLOTS; i++) {
		app_push(&free_slot_stack, i+1);	
	}
	printf("Parking slots initialized; stack top is now %d.\n", free_slot_stack.top);
	
	// Get ready the stacks for the car threads:
	for (int i=1; i <= NR_OF_CARS; i++) {
		car_thread_ids[i-1] = k_thread_create(&car_threads[i-1], car_stacks[i-1], APP_STACK_SIZE, car_fcn, INT_TO_POINTER(i), (void *) CAR_PARK_TIME, (void *)CAR_DRIVE_TIME, APP_PRIORITY, K_USER, K_FOREVER);
		printf("Car %d thread created.\n", i);
	}
}

void start_cars(){
	init_cars();
	for (int i=0; i < NR_OF_CARS; i++) {
		k_thread_start(&car_threads[i]);
		printf("Car %d thread started.\n", i);
		k_sleep(K_MSEC(APP_THREAD_START_DELAY));
	}
}

void main() {
    printf("Booting Zephyr!\n");
	printf("Preparing the parking\n");
	init_cars();
	k_sleep(K_MSEC(1000));
	start_cars();	
	printf("All cars are now going around.\n");
}