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

===================================
TASK1: Elementary thread operations
===================================
-------------------------
TASK1.1 Your first thread
-------------------------

Start with the Thread description in `Zephyr online documentation.<https://docs.zephyrproject.org/latest/kernel/services/threads/index.html>`_

Write a code, which has just one thread created using the K_THREAD_DEFINE macro.
This thread displays a message such as "Hello from my thread" and then it sleeps for given interval in ms (use k_sleep function from API and K_MSEC macro).

If you do not know how to start, look to the thread example in samples directory (zephyr/samples/basic/threads).
If you are totally lost, inspire yourself in the solutions directory of this lab.

-----------------------------------
TASK1.2 Threads working in parallel
-----------------------------------

Add another thread to your solution, with different timing and different message. 
Again, if you're in a bind, check the solutions folder.

--------------------------------------------------------
TASK1.3 Threads working in parallel, one entry point fcn
--------------------------------------------------------

The file task_1.2.c in solutions is ugly: both threads have unique entry points, but in fact, they are doing the same work, just with different parameters.
Update the code, to have one template for the entry point fcn, with parameters defining the message string and the sleep interval.

========================
TASK2: User mode threads
========================

Again, `keep open the original docs:<https://docs.zephyrproject.org/latest/kernel/usermode/overview.html>`_
The samples directory has four nice `examples for user-space threads.<zephyr/samples/kernel/usermode>`_

---------------------------------
TASK2.1: Setting up for user-mode
---------------------------------

The config option CONFIG_USERSPACE is not enabled by default, so you need to enable it (in your prj.conf, or by menuconfig or other configuration method).

Enable also CONFIG_ASSERT to allow the kernel to check the parameters of the system calls.

For user-space, the CPU has to have at least the Memory Protection Unit (MPU) or Memory Management Unit (MMU) enabled. If you are not sure, what the MMU and MPU means, `Wikipedia gives short summary.<https://en.wikipedia.org/wiki/Memory_protection_unit>`_ 

The arm cortex m0 and m3 have not the MPU, so for playing with the user-space, we will use the qemu_cortex_r5 board.

To be sure, that the user-mode is working, compile and test the hello-world example for qemu_cortex_r5:

.. code-block:: shell

    west build -p always -b qemu_cortex_r5 -t run ./samples/kernel/usermode/hello_world 

Than test the same example with the qemu_cortex_m0. You shall see the error message:

.. code-block:: shell

 *** Booting Zephyr OS build zephyr-v3.5.0-3753-gc68564bc9346 ***
Hello World from privileged mode. (qemu_cortex_m0)
ASSERTION FAIL [k_is_user_context()] @ WEST_TOPDIR/zephyr/samples/userspace/hello_world_user/src/main.c:19
	User mode execution was expected

If you have not the ASSERTION FAIL message for cortex_m0, you have to set the CONFIG_ASSERT in your configuration.

------------------------------------
TASK2.1: User-mode threads in action
------------------------------------




