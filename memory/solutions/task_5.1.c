/*
 * Memory lab, TASK5.1: one producer, one consumer, one slab block.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

#define BLOCKS 1
#define BLOCK_SIZE 32
#define STACK_SIZE 1024
#define PRIORITY 5

K_MEM_SLAB_DEFINE(slab, BLOCK_SIZE, BLOCKS, 4);
K_MSGQ_DEFINE(queue, sizeof(char *), BLOCKS, 4);

void producer(void *p1, void *p2, void *p3)
{
	char *block = NULL;

	for (int round = 1; round <= 2; round++) {
		k_mem_slab_alloc(&slab, (void **)&block, K_FOREVER);
		snprintf(block, BLOCK_SIZE, "producer, round %d", round);
		k_msgq_put(&queue, &block, K_FOREVER);
		k_sleep(K_MSEC(100));
	}
}

K_THREAD_DEFINE(producer_tid, STACK_SIZE, producer, NULL, NULL, NULL, PRIORITY, 0, 0);

int main(void)
{
	char *block;

	for (int i = 0; i < 2; i++) {
		k_msgq_get(&queue, &block, K_FOREVER);
		printf("consumer got: %s\n", block);
		k_mem_slab_free(&slab, block);
	}
	return 0;
}
