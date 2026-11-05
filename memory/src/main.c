/*
 * Memory lab, TASK1: where code, data and the stack live.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>

int initialized = 42;
int zeroed;
const char greeting[] = "Hello";

int main(void)
{
	int local = 1;
	static int counter;

	printf("code   (main):        %p\n", (void *)main);
	printf("const  (greeting):    %p\n", (void *)greeting);
	printf("data   (initialized): %p\n", (void *)&initialized);
	printf("bss    (zeroed):      %p\n", (void *)&zeroed);
	printf("static (counter):     %p\n", (void *)&counter);
	printf("stack  (local):       %p\n", (void *)&local);
	return 0;
}
