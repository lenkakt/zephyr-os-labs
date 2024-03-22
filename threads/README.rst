*************************
Threads: An Introduction 
*************************


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

    west build -p always -b qemu_cortex_m0 -t run ./threads

All these labs shall be done using qemu_cortex_m0, on x86 emulated machine, sleeping and waiting does not work well (it is too fast).

=========================
TASK1.1 Your first thread
=========================   

Start with the Thread description in `Threads documentation. <https://docs.zephyrproject.org/latest/kernel/services/threads/index.html>`_

Write a code, which has just one thread created using the K_THREAD_DEFINE macro.
This thread displays a message such as "Hello from my thread" and then it sleeps for given interval in ms (use k_sleep function from API and K_MSEC macro).

If you do not know how to start, look to the thread example in samples directory.
If you are totally lost, inspire yourself in the solutions directory of this lab.

===================================
TASK1.2 Threads working in parallel
===================================

Add another thread to your solution, with different timing and different message. 
Again, if you're in a bind, check the solutions folder.

========================================================
TASK1.3 Threads working in parallel, one entry point fcn
========================================================

The file task_1.2.c in solutions is not kind of ugly: both threads have unique entry points, but in fact, they are doing exactly the same work, just with different parameters.
Update the code, to have one entry point function and use parameters to specify the interval and the message.

========================
TASK2 User space threads
========================

Start with the User mode description in `Zephyr online documentation. <https://docs.zephyrproject.org/latest/kernel/usermode/index.html>`_

------------------------------------------------------
TASK2.1 User space threads depend on MPU (MMU) support
------------------------------------------------------

The user-space threads can run only on systems with MPU (Memory Protection Unit) or MMU (Memory Management Unit) enabled. If you are not sure, what it is, `check the Wikipedia. <https://en.wikipedia.org/wiki/Memory_protection_unit#:~:text=MPU%20is%20a%20trimmed%20down,unit%20like%20virtual%20memory%20management.>`_

The board used in pervious example has no MPU, so in this lab we will use the qemu_cortex_r5 board.

In the `samples directory <zephyr/samples/userspace>`_ you can find four elementary examples demonstrating the user-space threads.

To have user-space working, you need to enable CONFIG_USERSPACE. You also need to enable CONFIG_ASSERT to enable the system check the permissions.

To test that your configuration is working, just move the main.c from the `hello-word sample <zephyr/samples/userspace/hello_world_user>`_ to your local main.c.

Compile it for qemu_cortex_r5 and run:

... code-block:: shell

    west build -p always -b qemu_cortex_r5 -t run ./threads


Try to do the same for qemu_cortex_m0:
... code-block:: shell

    west build -p always -b qemu_cortex_m0 -t run ./threads

You shall see the ASSERT message that the user-space is not supported on this board.
Try to switch off the CONFIG_ASSERT in your configuration and try to run the example at cortex_m0 again. See? It looks like user-space is working, but in fact... no! Take care in your real applications.

--------------------
TASK2.2 System calls
--------------------


