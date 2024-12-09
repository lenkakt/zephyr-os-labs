**************
The memory lab
**************

This lab is designed to help you understand how memory is managed in Zephyr OS. 
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

Memory in Zephyr is managed by the kernel. The kernel provides a set of APIs to allocate and free memory.
This task focuses to simple heaps memory allocation and deallocation.

1. Go through the documentation: https://docs.zephyrproject.org/latest/kernel/memory_management/heap.html
2. Write a simple code that allocates memory using k_malloc() and frees it using k_free(). 
3. Run the code on the board and check the memory allocation and deallocation.
4. Do some experiments: switch off freeing the memory - what happends? Try to allocate more than available - what happends?
   




