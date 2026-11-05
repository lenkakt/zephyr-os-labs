/*
 * Memory lab, TASK2.3: fragmentation, enough free memory but not in one piece.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

#define SMALL 256
#define COUNT 12
#define BIG 1024

int main(void)
{
	char *blocks[COUNT];
	char *big = NULL;
	int allocated = 0;

	for (int i = 0; i < COUNT; i++) {
		blocks[i] = k_malloc(SMALL);
		if (blocks[i] != NULL) {
			allocated++;
		}
	}
	printf("allocated %d blocks of %d bytes\n", allocated, SMALL);

	for (int i = 0; i < COUNT; i += 2) {
		k_free(blocks[i]);
		blocks[i] = NULL;
	}
	printf("freed every second block: %d bytes free in pieces\n", (allocated / 2) * SMALL);

	big = k_malloc(BIG);
	printf("%d bytes in one piece: %s\n", BIG, big != NULL ? "ok" : "allocation failed");

	for (int i = 1; i < COUNT; i += 2) {
		k_free(blocks[i]);
	}
	big = k_malloc(BIG);
	printf("after freeing everything, %d bytes: %s\n", BIG, big != NULL ? "ok" : "allocation failed");
	return 0;
}
