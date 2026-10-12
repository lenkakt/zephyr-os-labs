# Kernel lab - solutions and answers

## Files

| File | Task | How to use |
|---|---|---|
| `task_3.1.c` | TASK3.1 | rename to `main.c` and move to `src/` |
| `task_3.2.c` | TASK3.2 | rename to `main.c` and move to `src/` |
| `Kconfig_3.2` | TASK3.2 | rename to `Kconfig` and move to the `kernel_lab` directory |
| `prj_3.2.conf` | TASK3.2 | rename to `prj.conf` |

The numbers below were measured with Zephyr 4.4 and qemu_cortex_m0; other versions give
slightly different values, the proportions stay the same.

## TASK1.1

* `zephyr.elf` - the program with symbols and debugging information (used by the debugger,
  by `nm`, by the emulator).
* `zephyr.bin` - the raw content of the flash memory, no symbols; `zephyr.hex` - the same in
  the Intel HEX text format. One of these is flashed to a real device.
* `zephyr.map` - the linker map: where in memory each function and variable is.
* Memory report of the unmodified application: FLASH 15 060 B, RAM 4 056 B.

## TASK1.2

`rom_report` of the unmodified application (15 060 B in total):

| Part | Size | Share |
|---|---|---|
| `kernel` | 3 628 B | 24 % |
| `drivers` | 3 110 B | 21 % |
| `arch` | 1 016 B | 7 % |
| modules (WORKSPACE, HAL) | 3 038 B | 20 % |
| C library (`printf` etc., under "no paths") | ~1 750 B | ~12 % |
| application `main.c` | 16 B | 0.1 % |

With the timer application (TASK3.1) the kernel grows to 5 100 B (30 %) and `main.c` has
152 B (0.9 %).

* `nm` shows `main`, `z_cstart`, `z_impl_k_sleep` and other kernel functions in the same
  `zephyr.elf`.
* Calling a kernel function is an **ordinary function call** - no system call, no switch of the
  processor mode. The application runs in the same address space and the same privileged mode
  as the kernel, so a bug in the application can overwrite the kernel data.
  This is why a small kernel is not the same as a microkernel: architecturally it is closer
  to a monolithic kernel. (Zephyr can run threads in user mode - see the Threads lab - but it
  is optional and needs MPU/MMU.)

## TASK1.3

* qemu_x86: no `zephyr.bin` is generated - the x86 emulator boots `zephyr.elf` directly, there
  is no flash memory to fill.
* Memory report: RAM 106 656 B (the whole program runs from RAM); `size zephyr.elf` gives
  text 43 740 B, data 5 432 B, bss 57 472 B. On cortex_m0, text is 14 896 B.
* Why bigger: a different instruction set, and the board defconfig enables more features -
  e.g. `CONFIG_X86_MMU` (page tables), `CONFIG_X86_DEBUG_INFO`, `CONFIG_SCHED_SCALABLE`
  (red-black tree ready queue instead of a simple list).

## TASK2

| Configuration | FLASH | RAM | `kernel` in rom_report |
|---|---|---|---|
| default | 15 060 B | 4 056 B | 3 628 B |
| `CONFIG_MULTITHREADING=n` | 11 596 B (−23 %) | 3 504 B | 748 B |

The kernel shrinks to about one fifth: the scheduler, context switching, thread management,
the idle thread and the system work queue are gone. The drivers stay almost the same.

## TASK3.1

* With multi-threading off, the timer application **fails to link**:
  `undefined reference to 'k_work_submit'`. The work queue is a thread - without threads,
  the kernel does not contain this API at all. The configuration did not just make the kernel
  smaller, it removed a part of its API.
* With multi-threading on, the message is printed every second.
* The timer handler runs in the interrupt context; it only submits the work, the system work
  queue thread prints the message. (`printf` directly in the handler often seems to work in
  QEMU, but blocking or slow operations in an interrupt handler delay all other interrupts -
  and some kernel functions must not be called from an interrupt at all.)

## TASK3.2

* The application's `Kconfig` must end with `source "Kconfig.zephyr"`, otherwise all the Zephyr
  options disappear.
* The option appears in menuconfig in the "ZEPHYR-OS-LABS" menu and as
  `#define CONFIG_KL_TIMER_SEC 3` in `autoconf.h` (with `prj_3.2.conf`).
