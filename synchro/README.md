# Synchronization

This lab is dedicated to locks and synchronization primitives in Zephyr.

## Before you start

Make sure you're in the `zephyr-os-labs` directory with the virtual environment activated.

Build the example, clean all the old stuff and ensure that it is working:

```shell
west build -p always -b qemu_cortex_m0 -t run ./synchro
```

## TASK1: Mutexes

As ususal, it is a good idea to start with the documentation: https://docs.zephyrproject.org/latest/kernel/services/synchronization/mutexes.html

### TASK1.1: Mutexes in action

And now write a simple program which have 2 threads who share one mutex and one entry point function.
The function has 2 parameters: Thread number and the time to hold the mutex.
The function contains a loop where a thread checks if the mutex is locked, if not, it locks it and prints the message that it holds the mutex.
If mutex is already locked, it prints the message that it is waiting for the mutex and it waits for a short time and the cycle goes again.

### TASK1.2: Mutexes and the shared resource

Our shared resource is a simple counter (an integer variable).
You have two threads: one is incrementing the counter, and the second is decrementing it.
The threads are running in parallel, so you need to protect the counter with a mutex.
The threads are running in a loop, so you can see the effect of the mutex.

## TASK2: Semaphores

Again, we start with the documentation: https://docs.zephyrproject.org/latest/kernel/services/synchronization/semaphores.html

### TASK2.1: Semaphores: a parking lot

We have a parking lot with just 5 places and 10 cars.
A car is represented by a thread; all threads share one entry point function with three parameters: the car number, the time to park and the time spent outside the parking lot.
You need to implement a data structure to keep the information about free places in the parking lot; a stack is highly recommended here. All the threads are using this structure, so you have to protect it with a mutex.

A car function does following in cycle:

1. A car arrives at the parking lot and tries to park. It checks the sempahpore if there is a free place.
2. If there is a free place, it finds out which place is free and parks there. It prints the message that it is parking and sleeps for given parking time.
3. When parking is done, the car first releases the slot and then the semaphore. It prints the message that it is leaving the parking lot and sleeps for the given time outside the parking lot.
