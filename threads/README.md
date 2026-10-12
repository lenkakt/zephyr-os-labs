# Threads: An Introduction

## Threads Lab goals:

1. To create and run your own threads using the K_THREAD_DEFINE macro.
2. To let threads communicate with each other using a pipe.
3. To understand Zephyr's userspace threads and why they need explicit permission to access kernel objects.

## Before you start

Make sure you are in the `zephyr-os-labs` directory with the virtual environment activated.

Build the example from scratch (`-p always` removes the old build) and check that it works:

```shell
west build -p always -b qemu_cortex_m0 -t run threads
```

Use qemu_cortex_m0 or any other Arm Cortex-M board for this lab. Sleeping and waiting do not work well on qemu_x86 (it is too fast).

## Initial note

In this lab, you will write more complex code than in the previous labs. If you are unsure and you are working in a lab with a tutor, do not hesitate to ask the tutor for help. You can also check the solutions for inspiration.

Copy-pasting code from the solutions or asking an AI agent to write the code for you, instead of writing it yourself, keeps you from real learning. Be brave and try it yourself.

## TASK1: Threads and how they communicate

### TASK1.1: Your first thread

Start with the Thread description in [Threads documentation.](https://docs.zephyrproject.org/latest/kernel/services/threads/index.html)

Write code that has just one thread created using the K_THREAD_DEFINE macro.
This thread displays a message such as "Hello from my thread". Then, it sleeps for the given interval in ms (use the k_sleep function from the API and the K_MSEC macro).

Note on priorities: a lower number means a higher priority. Threads with a non-negative priority (such as 5 in the solutions) are preemptive - the scheduler may switch them out at any time for a thread with a higher priority. Threads with a negative priority (such as -1 in the solutions of TASK1.4, TASK1.5 and TASK2) are cooperative - they run until they give up the processor themselves, e.g. by calling k_sleep or by waiting for a kernel object. See [Scheduling](https://docs.zephyrproject.org/latest/kernel/services/scheduling/index.html) in the Zephyr documentation.

If you do not know how to start, look at the [thread example](zephyr/samples/basic/threads) in the samples directory.
If you need help, look for inspiration in the solutions directory of this lab.

### TASK1.2: Threads working in parallel

Add another thread to your solution with different timing and messages.
Again, if you're in a bind, check the solutions folder.

### TASK1.3: Threads working in parallel, one entry point fcn

The solution of TASK1.2 is just written in a hurry: both threads have unique entry points, but in fact, they are doing the same work, just with different parameters.
Update the code. You want one entry point function with parameters specifying the interval and the message.

### TASK1.4: Threads and pipe

In Zephyr, threads can communicate in many ways. We will implement a simple one: using a pipe.
A pipe is a byte stream where one thread can write, and another can read. The pipe is a part of the kernel services so you can use it in your code.
Read the [pipe kernel object documentation.](https://docs.zephyrproject.org/latest/kernel/services/data_passing/pipes.html)
Write a straightforward application with two threads. The first thread writes a message to the pipe, and the second reads and displays it.
As usual, you have a simple solution in the solutions directory.

Note: the pipe API was reworked in Zephyr 4.1/4.2. The old `CONFIG_PIPES` option
and the `k_pipe_put()`/`k_pipe_get()` functions are deprecated and were removed
in Zephyr 4.3. This lab uses the current API - `k_pipe_write()` and
`k_pipe_read()` - which is what the documentation linked above and the
solutions in this repository use. If you find older material online still
referencing `CONFIG_PIPES`, `k_pipe_put()`, or `k_pipe_get()`, it targets an
older Zephyr version and will not work with the Zephyr version used here.

### TASK1.5: Waiting for a thread

In the solution of TASK1.4, main sleeps for 5 seconds and hopes that the producer is done by then.
If the producer needs more time, the consumer finds the pipe empty; if it needs less, the time is wasted.
A thread can instead wait until another thread ends: it calls `k_thread_join` and is blocked until then.
Read about [k_thread_join](https://docs.zephyrproject.org/latest/kernel/services/threads/index.html) in the documentation.

Update your code from TASK1.4:

1. Let the producer simulate some work: it sleeps for 2 seconds before it writes to the pipe.
2. Replace the `k_sleep` in main with `k_thread_join` on the producer thread.
3. Start the consumer and let main wait for it with `k_thread_join` too. Then main prints a final message.
4. Print the time from the start of main (`k_uptime_get`) when each thread finishes.
   Does main wait exactly as long as the producer works?

## TASK2: Userspace threads

Start with the User mode description in [Zephyr online documentation.](https://docs.zephyrproject.org/latest/kernel/usermode/index.html)

Note: a Zephyr *userspace thread* is not a *user-level thread*.
In general OS terminology, user-level threads are switched by a library and the kernel does not know about them.
In Zephyr, all threads - kernel-space and userspace - are created and scheduled by the kernel.
The `K_USER` flag only makes the thread run in the unprivileged mode of the processor:
it can access only its own memory and the kernel objects it has been granted access to.

### TASK2.1: Userspace threads depend on MPU (MMU) support

The userspace threads can run only on systems with an MPU (Memory Protection Unit) or MMU (Memory Management Unit) enabled. If you are not sure what it is, [check Wikipedia.](https://en.wikipedia.org/wiki/Memory_protection_unit#:~:text=MPU%20is%20a%20trimmed%20down,unit%20like%20virtual%20memory%20management.)

The board used in the previous example has no MPU, so we will use the qemu_cortex_r5 board in this lab.

In the [samples directory](zephyr/samples/userspace), you can find four elementary examples demonstrating the userspace threads.

To have userspace working, you need to enable CONFIG_USERSPACE. You also need to enable CONFIG_ASSERT to enable the system to check the permissions.

To test your configuration, move the main.c from the [hello-world sample](zephyr/samples/userspace/hello_world_user) to your local main.c.

Compile it for qemu_cortex_r5 and run:

```shell
west build -p always -b qemu_cortex_r5 -t run threads
```

Try to do the same for qemu_cortex_m0:

```shell
west build -p always -b qemu_cortex_m0 -t run threads
```

You shall see the ASSERT message informing you that this board does not support userspace.
Switch off the CONFIG_ASSERT in your configuration and run the example at cortex_m0 again. See? It looks like userspace is working, but in fact... no! Take care in your applications.

### TASK2.2: Your first userspace thread

Based on experience from the samples, write your first code spawning the two threads, one working in userspace and the other in kernel space.
Both threads shall display a message and return.
In case of trouble, see the example solution in the solutions directory. Try to find a difference between the kernel-space and userspace threads creation.

### TASK2.3: Userspace threads and pipes

Take your code with 2 threads with pipe and `k_thread_join` (Task 1.5) and move the consumer to userspace.
Run the code.
As you see, the consumer is not working.
The output stops after `Consumer is starting.` and a dump of processor registers follows,
ending with `Faulting instruction address (r15/pc): 0x00000000`.
The whole system halts, so main never prints its final message.
The dump does not tell you what went wrong.
The problem is that the userspace consumer cannot access the kernel object - the lab_pipe.
Someone must grant access to the pipe.
Update your code - the main thread shall grant access to lab_pipe to the consumer thread before this one is started.
