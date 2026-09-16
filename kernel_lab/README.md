# Playing with the Zephyr kernel

## Kernel Lab goals:

1. To understand the build products and how they are related to selected board architecture.
2. To play with the kernel configuration and see how it affects the size and other parameters of a builded system.
3. To play with kernel timers to understand how we work with the kernel API.

## Before you start

Make sure you're in the `zephyr-os-labs` directory with the virtual environment activated.

Build the example as it is to be sure it is working:

```shell
west build -p always -b qemu_cortex_m0 -t run kernel_lab
```

## TASK1: The Build directory

The goal is to answer the following questions: Where are the binaries? How big are they? What else is created during translation? Why so many files? How can I use it? Etc.

Read or at least srcroll through the original [documentation](https://docs.zephyrproject.org/latest/build/cmake/index.html).

Our current build directory is located in zephyr-os-labs/build/

1. Try to understand the meaning of all the zephyr*.* files (with the help of the documentation).
2. Try to find a file which will be used for flashing the device.
3. Note the size of `zephyr.bin` for qemu_cortex_m0 (the build you just did above).
4. Now build the same, unmodified application for qemu_x86:

   ```shell
   west build -p always -b qemu_x86 -t run kernel_lab
   ```

   Compare the build directory and `zephyr.bin` size against the cortex_m0
   build. Same source code, same options - the difference you see is purely
   the board architecture.
5. Rebuild the example for the cortex_m0 again, so the following tasks start
   from that board.

## TASK2: Kernel configuration & build outputs

The goal is to verify that the configuration of the system affects the size of the final binaries and to play with the system configuration.

1. Build the system as it is for the qemu_cortex_m0. Check the zephyr.bin size.
2. Start the guiconfig (or menuconfig if working in a terminal):

```shell
west build -b qemu_cortex_m0 -t guiconfig
```

3. In the General Kernel Options switch off the "Multi-threading" option. Exit config and compile again.

```shell
west build -b qemu_cortex_m0 -t run kernel_lab
```

Check the zephyr.bin size again (or just check the the flash memory report in the command line).

## TASK3: Simple intro to kernel APIs - timers

This part aims to demonstrate how to work with any kernel API. There are separate lessons for threads, thread scheduling, locks, events, and other interesting parts of the kernel API, and we'll start by playing with the timer API.

Select the qemu_cortex_m0 for the emulation. The qemu_x86 machine is not working well with timers.

As usual, start with the [documentation](https://docs.zephyrproject.org/latest/kernel/services/timing/timers.html).

### TASK3.1: A simple timer

Prepare a simple application using one timer. The timer periodically displays the "Hello world" message into a serial port. Set the timing as you want.

### TASK3.2: Make the interval configurable

Make the timer period a configuration option of your application. Try to create a Kconfig file containing a menu called "ZEPHYR-OS-LABS" and define an interval configuration option here. Use it in your main.c to set the interval. Play with (gui)(menu)config, and also try to define the value in prj.conf file as well.

In any case of trouble, look to the solutions directory for inspiration.
