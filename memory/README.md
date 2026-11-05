# Memory

## Memory Lab goals:

1. To find out where code, data and the stack of a program live in memory.
2. To allocate memory from the heap and see when the allocation fails.
3. To see a memory leak and fragmentation.
4. To measure how much stack a thread needs and what happens when it is too small.
5. To see how the MPU stops a thread that writes where it must not, and how to give the thread access.
6. To pass fixed-size memory blocks (slabs) between threads.

## Before you start

Make sure you are in the `zephyr-os-labs` directory with the virtual environment activated.

Build the example from scratch and check that it works:

```shell
west build -p always -b qemu_cortex_m0 -t run memory
```

## Initial note

All boards in this lab are emulated by QEMU, you do not need any hardware. Most tasks use `qemu_cortex_m0`. Two other boards are used where the default one is not enough:

- `qemu_cortex_m3` in TASK1.2 and TASK3.1, to compare the results with another processor,
- `mps2/an385` in TASK3.2 and TASK4, because it has an MPU (Memory Protection Unit) and supports user mode threads.

To use another board, change the `-b` parameter, for example:

```shell
west build -p always -b mps2/an385 -t run memory
```

Always build with `-p always` when you change the board or the configuration.

Some tasks need configuration options. Add them to `memory/prj.conf`, one option per line. The options for each task are also in `solutions/prj_*.conf`.

In TASK1, you only run and change the prepared program in `src/main.c`. From TASK2 on, you write your own programs – replace `src/main.c` with your code, or create a new application for each task.

The programs end, but QEMU keeps running. Press Ctrl+A, then X, to quit it.

As in the previous labs, try to write the code yourself before you look at the solutions.

## TASK1: Where does my program live?

### TASK1.1: Addresses

Read `src/main.c`. It prints the addresses of a function, a constant, two global variables, a static local variable and a local variable. Build and run it.

When the build finishes, it prints a table like this one:

```
Memory region         Used Size  Region Size  %age Used
           FLASH:       15184 B       256 KB      5.79%
             RAM:        4072 B        16 KB     24.85%
```

The `qemu_cortex_m0` board has 256 KB of flash starting at address `0x00000000` and 16 KB of RAM (SRAM) starting at `0x20000000`. Answer these questions:

1. Which of the printed addresses are in flash and which are in RAM? Why is the constant `greeting` in flash, but the variable `initialized` in RAM?
2. The variables `initialized` and `zeroed` are both global. Why are they stored in different places? Hint: look for the sections `.data` and `.bss`.
3. Where does the static local variable `counter` live: on the stack, or with the global variables?
4. The local variable `local` is in RAM, too. Whose stack is it on?

### TASK1.2: The same addresses every time

Run the program again. Are the addresses the same?

There is no virtual memory on this board. The addresses you see are real physical addresses, and the linker decided them when you built the program. Nothing changes them when the program starts.

Build the program for `qemu_cortex_m3` and compare the addresses with the ones from `qemu_cortex_m0`. Which of them changed and which stayed the same? Why?

## TASK2: The heap

Zephyr can allocate memory while the program runs. The functions `k_malloc()` and `k_free()` work with the system heap. Read the documentation first: https://docs.zephyrproject.org/latest/kernel/memory_management/heap.html

### TASK2.1: Allocating and freeing

Write a program that allocates a block of 512 bytes with `k_malloc()`, prints its address and frees it with `k_free()`. Repeat it ten times in a loop.

Replace the content of `src/main.c` with this template and complete the two lines marked `TODO`:

```c
#include <stdio.h>
#include <zephyr/kernel.h>

#define BLOCK_SIZE 512
#define ROUNDS 10

int main(void)
{
	for (int i = 1; i <= ROUNDS; i++) {
		char *block = NULL; /* TODO: allocate BLOCK_SIZE bytes with k_malloc() */

		if (block == NULL) {
			printf("round %d: allocation failed\n", i);
		} else {
			printf("round %d: %d bytes at %p\n", i, BLOCK_SIZE, (void *)block);
			/* TODO: free the block with k_free() */
		}
	}
	return 0;
}
```

Build the program. What happened? The system heap is not there by default: its size, `CONFIG_HEAP_MEM_POOL_SIZE`, is zero. Add this line to `memory/prj.conf`:

```
CONFIG_HEAP_MEM_POOL_SIZE=4096
```

Build and run the program again. What address do the blocks get?

### TASK2.2: A memory leak

Remove `k_free()` from your program and run it again.

1. How many blocks did you get before the allocation failed?
2. 4096 / 512 = 8. Why did you get fewer? Look at the addresses: how far apart are two blocks?
3. What does `k_malloc()` return when there is no memory left? Does your program check it?

### TASK2.3: Fragmentation

Write a program that:

1. allocates twelve blocks of 256 bytes,
2. frees every second block,
3. tries to allocate one block of 1024 bytes,
4. frees all the remaining blocks and tries to allocate 1024 bytes again.

Replace the content of `src/main.c` with this template and complete the lines marked `TODO`:

```c
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
		/* TODO: free blocks[i] with k_free() */
		blocks[i] = NULL;
	}
	printf("freed every second block: %d bytes free in pieces\n", (allocated / 2) * SMALL);

	/* TODO: allocate BIG bytes with k_malloc() into big */
	printf("%d bytes in one piece: %s\n", BIG, big != NULL ? "ok" : "allocation failed");

	for (int i = 1; i < COUNT; i += 2) {
		k_free(blocks[i]);
	}
	/* TODO: allocate BIG bytes with k_malloc() into big again */
	printf("after freeing everything, %d bytes: %s\n", BIG, big != NULL ? "ok" : "allocation failed");
	return 0;
}
```


How many bytes are free after step 2? Why does the allocation in step 3 fail, and why does it work in step 4?

## TASK3: The stack

Every thread has its own stack, and in Zephyr you choose its size yourself, when you build the program. Too much wastes RAM, too little breaks the program.

### TASK3.1: How much stack does a thread use?

Create two threads with 1024 bytes of stack each. The first thread does nothing, the second one prints a line with `printf()` and two numbers.

When both threads finish, ask how much of their stack they used. The function `k_thread_stack_space_get()` tells you how many bytes of the stack were never touched.

Replace the content of `src/main.c` with this template and complete the two lines marked `TODO`:

```c
#include <stdio.h>
#include <zephyr/kernel.h>

#define STACK_SIZE 1024
#define PRIORITY 5

K_THREAD_STACK_DEFINE(quiet_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(talking_stack, STACK_SIZE);
struct k_thread quiet_thread, talking_thread;

void quiet(void *p1, void *p2, void *p3)
{
}

void talking(void *p1, void *p2, void *p3)
{
	/* TODO: print a line with two numbers using printf() */
}

static void report(const char *name, struct k_thread *thread)
{
	size_t unused = 0;

	/* TODO: get the unused stack space with k_thread_stack_space_get(thread, &unused) */
	printf("%-8s used %4u of %d bytes\n", name, (unsigned int)(STACK_SIZE - unused), STACK_SIZE);
}

int main(void)
{
	k_thread_create(&quiet_thread, quiet_stack, STACK_SIZE, quiet, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
	k_thread_create(&talking_thread, talking_stack, STACK_SIZE, talking, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
	k_thread_join(&quiet_thread, K_FOREVER);
	k_thread_join(&talking_thread, K_FOREVER);
	report("quiet", &quiet_thread);
	report("talking", &talking_thread);
	return 0;
}
```

For this, Zephyr must fill the stacks with a known value before the threads start. Add to `memory/prj.conf`:

```
CONFIG_INIT_STACKS=y
CONFIG_THREAD_STACK_INFO=y
```

1. How much stack does an empty thread need? What is in it?
2. How much more does one `printf()` take?
3. Build the program for `qemu_cortex_m3` and compare. Is the result the same?

### TASK3.2: Too small a stack

Create a thread with 1024 bytes of stack. In the thread, create a local array and fill it with `memset()`. Replace the content of `src/main.c` with this program:

```c
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

#define STACK_SIZE 1024
#define BUFFER_SIZE 800
#define PRIORITY 5

K_THREAD_STACK_DEFINE(stack, STACK_SIZE);
struct k_thread thread;

void worker(void *p1, void *p2, void *p3)
{
	volatile char buffer[BUFFER_SIZE];

	memset((char *)buffer, 1, BUFFER_SIZE);
	printf("Worker: buffer of %d bytes, first byte %d\n", BUFFER_SIZE, buffer[0]);
}

int main(void)
{
	k_thread_create(&thread, stack, STACK_SIZE, worker, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
	k_thread_join(&thread, K_FOREVER);
	printf("Main: worker finished\n");
	return 0;
}
```

Build the program for `mps2/an385` with the hardware stack protection. Add to `memory/prj.conf`:

```
CONFIG_HW_STACK_PROTECTION=y
```

1. Run the program with `BUFFER_SIZE` 800 and then 850. What happens? Why does 850 bytes not fit into a stack of 1024 bytes?
2. The protection works like a guard: the MPU watches a small area just below the stack. Try `BUFFER_SIZE` 2000. What happens now, and why is it worse?
3. Set `CONFIG_HW_STACK_PROTECTION=n` and try 850 and 1100 again. What does the program say now? Can you tell from the message what went wrong?

## TASK4: Protection

In Zephyr, a thread can run in user mode. The MPU then lets it access only its own stack and the memory it was explicitly given. Read the documentation: https://docs.zephyrproject.org/latest/kernel/usermode/memory_domain.html

Use `mps2/an385` and add to `memory/prj.conf`:

```
CONFIG_USERSPACE=y
```

### TASK4.1: Writing where you must not

Write a program with a global variable `secret`. The main thread sets it and prints it. Then it starts a user mode thread (the option `K_USER` in `k_thread_create()`), which tries to write to `secret`. The main thread waits for the user thread with `k_thread_join()`. Replace the content of `src/main.c` with this program:

```c
#include <stdio.h>
#include <zephyr/kernel.h>

int secret = 42;

#define STACK_SIZE 1024
K_THREAD_STACK_DEFINE(stack, STACK_SIZE);
struct k_thread thread;

void user_thread(void *p1, void *p2, void *p3)
{
	printf("User thread: trying to overwrite the secret\n");
	secret = 0;
	printf("User thread: done\n");
}

int main(void)
{
	secret = 43;
	printf("Kernel thread: secret is %d\n", secret);
	k_thread_create(&thread, stack, STACK_SIZE, user_thread, NULL, NULL, NULL, 5, K_USER, K_NO_WAIT);
	k_thread_join(&thread, K_FOREVER);
	printf("Kernel thread: secret is still %d\n", secret);
	return 0;
}
```

1. What happens? Which address does the MPU report, and what is at this address?
2. The whole system stops. Why is it better than letting the thread continue?

### TASK4.2: Giving access

Give the user thread access to `secret`. You need three things:

1. a memory partition with `secret` in it,
2. a memory domain with this partition, created with `k_mem_domain_init()`,
3. the user thread in this domain. Create the thread with the delay `K_FOREVER`, so that it does not start yet, add it to the domain with `k_mem_domain_add_thread()` and start it with `k_thread_start()`.

Replace the content of `src/main.c` with this template and complete the lines marked `TODO`:

```c
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/app_memory/app_memdomain.h>
#include <zephyr/sys/libc-hooks.h>

K_APPMEM_PARTITION_DEFINE(shared_part);
int secret = 42; /* TODO: put secret into shared_part with K_APP_DMEM(shared_part) */

#define STACK_SIZE 1024
K_THREAD_STACK_DEFINE(stack, STACK_SIZE);
struct k_thread thread;
struct k_mem_domain domain;

void user_thread(void *p1, void *p2, void *p3)
{
	printf("User thread: trying to overwrite the secret\n");
	secret = 0;
	printf("User thread: done\n");
}

int main(void)
{
	struct k_mem_partition *parts[] = { &shared_part };

	secret = 43;
	printf("Kernel thread: secret is %d\n", secret);
	/* TODO: create the domain with k_mem_domain_init(&domain, ARRAY_SIZE(parts), parts) */
	k_thread_create(&thread, stack, STACK_SIZE, user_thread, NULL, NULL, NULL, 5, K_USER, K_FOREVER);
	/* TODO: add the thread to the domain with k_mem_domain_add_thread(&domain, &thread) */
	k_thread_start(&thread);
	k_thread_join(&thread, K_FOREVER);
	printf("Kernel thread: secret is now %d\n", secret);
	return 0;
}
```

1. Run the program. The MPU stops the user thread again, even sooner than in TASK4.1: before it prints anything. Why? Hint: `printf()` is part of the C library, and the C library keeps its own data in the partition `z_libc_partition`. A thread in your domain can access only the partitions of the domain.
2. Add `&z_libc_partition` to the array `parts` and run the program again. Can the user thread write to `secret` now?

## TASK5: Slabs

A slab is a pool of memory blocks of one fixed size. Allocating and freeing a block is fast and the memory never gets fragmented. Slabs are used not only in Zephyr, but also in the Linux kernel and other operating systems.

1. To understand what a slab is, the English Wikipedia gives a quick summary: https://en.wikipedia.org/wiki/Slab_allocation
2. Now, check the Zephyr implementation: https://docs.zephyrproject.org/latest/kernel/memory_management/slabs.html

Go back to `qemu_cortex_m0`. Remove `CONFIG_HW_STACK_PROTECTION` and `CONFIG_USERSPACE` from `memory/prj.conf` first: this board has no MPU, and the build would only warn that it ignores them.

In this task, producers fill slab blocks with data and a consumer reads them. To pass a block from a producer to the consumer safely, use a message queue (`k_msgq`) that carries the pointer to the block. The consumer frees the block when it has read it.

### TASK5.1: Just one block

1. Create a slab with one block of 32 bytes.
2. Create a producer thread and use the main thread as the consumer.
3. The producer allocates the block, writes a text into it, puts the pointer into the queue and repeats it twice.
4. The consumer takes the pointer from the queue, prints the text and frees the block.

Replace the content of `src/main.c` with this template and complete the lines marked `TODO`:

```c
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
		/* TODO: allocate a block: k_mem_slab_alloc(&slab, (void **)&block, K_FOREVER) */
		snprintf(block, BLOCK_SIZE, "producer, round %d", round);
		/* TODO: put the pointer into the queue: k_msgq_put(&queue, &block, K_FOREVER) */
		k_sleep(K_MSEC(100));
	}
}

K_THREAD_DEFINE(producer_tid, STACK_SIZE, producer, NULL, NULL, NULL, PRIORITY, 0, 0);

int main(void)
{
	char *block;

	for (int i = 0; i < 2; i++) {
		/* TODO: take the pointer from the queue: k_msgq_get(&queue, &block, K_FOREVER) */
		printf("consumer got: %s\n", block);
		/* TODO: free the block: k_mem_slab_free(&slab, block) */
	}
	return 0;
}
```

What does the producer do while the consumer still has the only block?

### TASK5.2: More producers

1. Create a slab with four blocks.
2. Create four producer threads and one consumer.
3. Each producer writes its own number into the block.

In which order does the consumer get the data?

### TASK5.3: Producers in an array

Improve your solution from TASK5.2 to keep all the producers in an array. This way, you can change the number of producers by editing one parameter in your code. Hint: `K_THREAD_STACK_ARRAY_DEFINE`.
