/*
 * Memory lab, TASK5.2: four producers, one consumer.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

#define BLOCK_SIZE 32
#define BLOCKS 4
#define STACK_SIZE 1024
#define PRIORITY 5

K_MEM_SLAB_DEFINE(slab, BLOCK_SIZE, BLOCKS, 4);
K_MSGQ_DEFINE(queue, sizeof(char *), BLOCKS, 4);

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

K_THREAD_DEFINE(producer0, STACK_SIZE, producer, (void *)0, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(producer1, STACK_SIZE, producer, (void *)1, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(producer2, STACK_SIZE, producer, (void *)2, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(producer3, STACK_SIZE, producer, (void *)3, NULL, NULL, PRIORITY, 0, 0);

int main(void)
{
	char *block;

	for (int i = 0; i < 2 * BLOCKS; i++) {
		k_msgq_get(&queue, &block, K_FOREVER);
		printf("consumer got: %s\n", block);
		k_mem_slab_free(&slab, block);
	}
	return 0;
}
