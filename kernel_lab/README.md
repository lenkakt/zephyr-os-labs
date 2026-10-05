# Playing with the Zephyr kernel

## Kernel Lab goals

1. To find out what the build produces and to see that **the kernel and your application are
   one single program**.
2. To see how the kernel configuration changes the size of the system - and what it removes.
3. To use the first kernel API - timers - and to understand why the work in a timer handler
   should be short.

## Before you start

Make sure you're in the `zephyr-os-labs` directory with the virtual environment activated.
You should already know `prj.conf`, menuconfig and the files `.config` and `autoconf.h`
from the Config lab.

Build the example as it is to be sure it is working:

```shell
west build -p always -b qemu_cortex_m0 kernel_lab -t run
```

## TASK1: One program

In Linux, the kernel (`/boot/vmlinuz-...`) and the applications (`/usr/bin/...`) are separate
files, and an application calls the kernel through system calls. Let's see how it is in Zephyr.

Read or at least scroll through the [build system documentation](https://docs.zephyrproject.org/latest/build/cmake/index.html).

### TASK1.1: The build products

Look into `build/zephyr/` and find the files `zephyr.elf`, `zephyr.bin`, `zephyr.hex`
and `zephyr.map`.

1. Which of them contains debugging information and symbols, and which is just the raw content
   of the flash memory?
2. Which file would you use to flash a real device?
3. At the end of the build output, find the memory report:

   ```text
   Memory region         Used Size  Region Size  %age Used
              FLASH:       15060 B       256 KB      5.74%
                RAM:        4056 B        16 KB     24.76%
   ```

   Write down the FLASH and RAM usage.

### TASK1.2: What is inside

1. Let the build system break the flash usage down by source files:

   ```shell
   west build -d build -t rom_report
   ```

   Find the `kernel`, `drivers` and `arch` directories in the tree, and find your own `main.c`.
   How many bytes is the kernel? How many bytes is your application?

2. List the functions in `zephyr.elf`:

   ```shell
   nm build/zephyr/zephyr.elf | grep " T " | less
   ```

   Find your `main` and some kernel functions (e.g. `z_cstart` - the start of the kernel,
   `k_sleep` or `z_impl_k_sleep`). They are all in the same file.

Think about it: when your application calls a kernel function, is it a system call, or just
an ordinary function call? What does it mean for the protection of the kernel?

### TASK1.3: Another architecture

Build the same, unmodified application for qemu_x86:

```shell
west build -p always -b qemu_x86 kernel_lab -t run
```

Compare the memory report and the files in `build/zephyr/` with the cortex_m0 build.
Same source code, same options - the difference is caused purely by the board.
Which file is missing now, and why do you think it is so? Why is the system so much bigger?
(Hint: look into the board defconfig, as in the Config lab.)

## TASK2: Kernel configuration and size

1. Build the system for qemu_cortex_m0 again and note the FLASH and RAM usage:

   ```shell
   west build -p always -b qemu_cortex_m0 kernel_lab
   ```

2. Start the guiconfig (or menuconfig if working in a terminal):

   ```shell
   west build -b qemu_cortex_m0 kernel_lab -t guiconfig
   ```

3. In General Kernel Options, switch off the "Multi-threading" option. Save, exit and build again
   (without `-p always` - you would lose the menuconfig change, remember the Config lab):

   ```shell
   west build -b qemu_cortex_m0 kernel_lab -t run
   ```

4. Compare FLASH and RAM usage. Run `rom_report` again - which part of the system got smaller?
   What exactly disappeared from the kernel?

**Leave the multi-threading switched off** and continue with TASK3.

## TASK3: Simple intro to kernel APIs - timers

This part shows how to work with a kernel API. There are separate labs for threads, scheduling,
locks and other parts of the kernel API; we start with timers.

Use qemu_cortex_m0 for this task - time does not run correctly in the qemu_x86 emulation.

As usual, start with the [documentation](https://docs.zephyrproject.org/latest/kernel/services/timing/timers.html).

### TASK3.1: A simple timer

Prepare a simple application using one timer. The timer periodically writes a "Hello world"
message to the serial port. Set the timing as you want.

The timer handler (the expiry function) runs in the **interrupt context**: it is called from the
timer interrupt, while the interrupted code waits. That's why it should be very short.
Writing a message to the serial port is slow, so it is better to hand it over to a thread.
Zephyr has a ready-made thread for such work: the system work queue (`k_work`, see the
[workqueue documentation](https://docs.zephyrproject.org/latest/kernel/services/threads/workqueue.html)).
The timer handler only submits the work, the work queue thread writes the message later.

1. Write the application with the timer and the work queue and build it - remember, the
   multi-threading is still switched off from TASK2. What happened? Read the error message.
   Why does the kernel not know the function `k_work_submit`?
2. Switch the multi-threading on again (menuconfig, or rebuild with `-p always`) and run your
   application.

In any case of trouble, look to the solutions directory for inspiration.

### TASK3.2: Make the interval configurable

Make the timer period a configuration option of your application, the same way the kernel
options work. Create a `Kconfig` file in the `kernel_lab` directory containing a menu called
"ZEPHYR-OS-LABS" and define an interval option there. Do not forget to include the Zephyr
options at the end of your file:

```kconfig
source "Kconfig.zephyr"
```

Use the option in your `main.c` to set the interval. Find your option in menuconfig,
set it in `prj.conf` as well, and check `autoconf.h` - your option is there, next to the kernel
options.

In any case of trouble, look to the solutions directory for inspiration.
