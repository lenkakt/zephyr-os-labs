**************
The memory lab
**************

This lab demonstrates several different memory management techniques used in Zephyr.
It will help you understand the memory layout of Zephyr OS and how to use the memory management APIs.

================
Before you start
================

1. Unless otherwise specified, it is assumed that you enter commands in the zephyr-os-labs directory.
2. Do not forget to activate the environment using command: 

.. code-block:: shell

 cd zephyr-os-labs
 source .venv/bin/activate

3. Build the example, clean all the old stuff and ensure that it is working:

.. code-block:: shell

 west build -p always -b qemu_cortex_m0 -t run ./memory

===============================
TASK1: Simple memory allocation 
===============================

The kernel manages memory in Zephyr. It provides a set of APIs to allocate and free memory.
This task focuses on simple heaps of memory allocation and deallocation.

1. Go through the documentation: https://docs.zephyrproject.org/latest/kernel/memory_management/heap.html
2. Write a simple code that allocates memory using k_malloc() and frees it using k_free(). 
3. Run the code on the board and check the memory allocation and deallocation.
4. Do some experiments: switch off freeing the memory - what happens? Try to allocate more than is available - what happens?

======================================
TASK2: Slabs, shared memory in threads
======================================

Slabs are a more efficient way to allocate memory. They allow dynamic allocation from a fixed-size memory pool. Slabs are not implemented only in the Zephyr OS; they can also be found in the Linux kernel and other types of operating systems.

1. To understand what the slab is, English Wikipedia dives quick summary: https://en.wikipedia.org/wiki/Slab_allocation
2. Now, check the Zephyr implementation: https://docs.zephyrproject.org/latest/kernel/memory_management/slabs.html

----------------------
TASK2.1: Just one slab
----------------------

1. To get familiar with the API, create two threads: a producer and a consumer.
2. You have one slab with one block.
3. The producer thread allocates the memory and fills it with some data.
4. The consumer thread reads the data and frees the memory.

----------------------
TASK2.2: More slabs
----------------------

1. Create a slab containing four blocks.
2. Create four producer threads and one consumer thread.
3. Each producer allocates one block and fills it with its unique data (e.g., thread ID).
4. The consumer reads the data and frees the memory.

Just note: you do not need to free the slab block every time you read it; you can keep it allocated and reuse it as you want.

------------------------------
TASK2.3: Threads in an array
------------------------------
Improve your solution from 2.2 to keep all the producers in an array. This way, you can easily change the number of producers by editing one parameter in your code or application configuration.


   




