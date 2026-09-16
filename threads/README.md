# Threads: An Introduction

## Before you start

Make sure you're in the `zephyr-os-labs` directory with the virtual environment activated.

Build the example, clean all the old stuff and ensure that it is working:

```shell
west build -p always -b qemu_cortex_m0 -t run ./threads
```

Use qemu_cortex_m0 or any other arm m cortex for those labs. Sleeping and waiting does not work well for qemu_x86 (it is too fast).

## TASK1.1 Your first thread

Start with the Thread description in [Threads documentation.](https://docs.zephyrproject.org/latest/kernel/services/threads/index.html)

Write a code that has just one thread created using the K_THREAD_DEFINE macro.
This thread displays a message such as "Hello from my thread". Then, it sleeps for the given interval in ms (use the k_sleep function from API and K_MSEC macro).

If you do not know how to start, look at the thread example in the samples directory.
If you need help, inspire yourself in the solutions directory of this lab.

### TASK1.2 Threads working in parallel

Add another thread to your solution with different timing and messages.
Again, if you're in a bind, check the solutions folder.

### TASK1.3 Threads working in parallel, one entry point fcn

The file task_1.2.c in solutions is just fastly written: both threads have unique entry points, but in fact, they are doing the same work, just with different parameters.
Update the code. You want one entry point function with parameters specifing the interval and the message.

### TASK1.4 Threads and pipe

In Zephyr, threads can communicate in many ways. We will implement a simple one: using a pipe.
A pipe is a text stream where one thread can write, and another can read. The pipe is a part of the kernel services so you can use it in your code.
Read the [pipe kernel object documentation.](https://docs.zephyrproject.org/latest/kernel/services/data_passing/pipes.html)
Write a straightforward application with two threads. The first thread writes a message to the pipe, and the second reads and displays it.
As usual, you have a simple solution in the solutions directory.

## TASK2 Userspace threads

Start with the User mode description in [Zephyr online documentation.](https://docs.zephyrproject.org/latest/kernel/usermode/index.html)

### TASK2.1 Userspace threads depend on MPU (MMU) support

The userspace threads can run only on systems with an MPU (Memory Protection Unit) or MMU (Memory Management Unit) enabled. If you are not sure what it is, [check Wikipedia.](https://en.wikipedia.org/wiki/Memory_protection_unit#:~:text=MPU%20is%20a%20trimmed%20down,unit%20like%20virtual%20memory%20management.)

The board used in the previous example has no MPU so that we will use the qemu_cortex_r5 board in this lab.

In the [samples directory](zephyr/samples/userspace), you can find four elementary examples demonstrating the userspace threads.

To have userspace working, you need to enable CONFIG_USERSPACE. You also need to enable CONFIG_ASSERT to enable the system to check the permissions.

To test your configuration, move the main.c from the [hello-word sample](zephyr/samples/userspace/hello_world_user) to your local main.c.

Compile it for qemu_cortex_r5 and run:

```shell
west build -p always -b qemu_cortex_r5 -t run ./threads
```

Try to do the same for qemu_cortex_m0:

```shell
west build -p always -b qemu_cortex_m0 -t run ./threads
```

You shall see the ASSERT message informing you that the userspace on this board does not support the userspace.
Switch off the CONFIG_ASSERT in your configuration and run the example at cortex_m0 again. See? It looks like userspace is working, but in fact... no! Take care in your applications.

### TASK2.2 Your first userspace thread

Based on experience from the samples, write your first code spawning the two threads, one working in userspace and the other in kernel space.
Both threads shall display a message and return.
In case of trouble, see solutions for my example. Try to find a difference between the kernel-space and userspace threads creation.

### TASK2.3 Userspace threads and pipes

Take your code with 2 threads with pipe (Task 1.4) and move the consumer to userspace.
Run the code.
As you see, the consumer is not working.
The problem is that the userspace consumer cannot access the kernel object - the lab_pipe.
Someone must grant access to the pipe.
Update your code - the main thread shall grant access to lab_pipe to the consumer thread before this one is started.

**Warning: This lab has updated solutions, in Zephyr 4.2 or newer the pipe API is different. Do
not use CONFIG_PIPES and check the solutions, if you still have older version of this lab at your computer.**
