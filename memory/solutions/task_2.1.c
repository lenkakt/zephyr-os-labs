/*
 * Memory lab, TASK2.1: allocating and freeing heap memory.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

#define BLOCK_SIZE 512
#define ROUNDS 10

int main(void)
{
	for (int i = 1; i <= ROUNDS; i++) {
		char *block = k_malloc(BLOCK_SIZE);

		if (block == NULL) {
			printf("round %d: allocation failed\n", i);
		} else {
			printf("round %d: %d bytes at %p\n", i, BLOCK_SIZE, (void *)block);
			k_free(block);
		}
	}
	return 0;
}
