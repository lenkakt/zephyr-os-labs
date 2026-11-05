/*
 * Memory lab, TASK5.3: producers in an array.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

#define PRODUCERS 4
#define BLOCK_SIZE 32
#define STACK_SIZE 1024
#define PRIORITY 5

K_MEM_SLAB_DEFINE(slab, BLOCK_SIZE, PRODUCERS, 4);
K_MSGQ_DEFINE(queue, sizeof(char *), PRODUCERS, 4);

K_THREAD_STACK_ARRAY_DEFINE(producer_stacks, PRODUCERS, STACK_SIZE);
struct k_thread producer_threads[PRODUCERS];

void producer(void *p1, void *p2, void *p3)
{
	int id = (int)(intptr_t)p1;
	char *block;

	for (int round = 1; round <= 2; round++) {
		k_mem_slab_alloc(&slab, (void **)&block, K_FOREVER);
		snprintf(block, BLOCK_SIZE, "producer %d, round %d", id, round);
		k_msgq_put(&queue, &block, K_FOREVER);
		k_sleep(K_MSEC(100));
	}
}

int main(void)
{
	char *block;

	for (int i = 0; i < PRODUCERS; i++) {
		k_thread_create(&producer_threads[i], producer_stacks[i], STACK_SIZE, producer,
				(void *)(intptr_t)i, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
	}
	for (int i = 0; i < 2 * PRODUCERS; i++) {
		k_msgq_get(&queue, &block, K_FOREVER);
		printf("consumer got: %s (free blocks: %u)\n", block, k_mem_slab_num_free_get(&slab));
		k_mem_slab_free(&slab, block);
	}
	return 0;
}
