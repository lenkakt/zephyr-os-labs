***************
Synchronization
***************

This lab is dedicated to locks and synchronization primitives in Zephyr. 

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

    west build -p always -b qemu_cortex_m0 -t run ./synchro


==============
TASK1: Mutexes
==============

As ususal, it is a good idea to start with the documentation: https://docs.zephyrproject.org/latest/kernel/services/synchronization/mutexes.html

--------------------------
TASK1.1: Mutexes in action
--------------------------

And now write a simple program which have 2 threads who share one mutex and one entry point function.
The function has 2 parameters: Thread number and the time to hold the mutex.
The function contains a loop where a thread checks if the mutex is locked, if not, it locks it and prints the message that it holds the mutex. 
If mutex is already locked, it prints the message that it is waiting for the mutex and it waits for a short time and the cycle goes again.

----------------------------------------
TASK1.2: Mutexes and the shared resource
----------------------------------------

Our shared resource is a simple counter (an integer variable).
You have two threads: one is incrementing the counter, and the second is decrementing it.
The threads are running in parallel, so you need to protect the counter with a mutex.
The threads are running in a loop, so you can see the effect of the mutex.


