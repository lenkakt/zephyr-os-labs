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

Start with the Thread description in `Zephyr online documentation.<https://docs.zephyrproject.org/latest/kernel/services/threads/index.html>`_

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

The file task_1.2.c in solutions is ugly: both threads have unique entry points, but in fact, they are doing the same work, just with different parameters.
Update the code, to have one template for the entry point fcn, with parameters defining the message string and the sleep interval.


