# Scheduling

In this lab, we will learn about scheduling in Zephyr. We will learn about the different types of scheduling policies and how to use them in Zephyr.

## Before you start

Make sure you're in the `zephyr-os-labs` directory with the virtual environment activated.

Build the example, clean all the old stuff and ensure that it is working:

```shell
west build -p always -b qemu_cortex_m0 -t run ./scheduling
```

## TASK1: Scheduling types in Zephyr

### TASK1.1: Scheduler configuration

Zephyr uses priority-based scheduler.
A detailed [descripition of the Zephyr scheduler](https://docs.zephyrproject.org/latest/kernel/services/scheduling/index.html) is available in the online docs.

Some scheduler features can be configured in the prj.conf file. The scheduler-related configuration options start with CONFIG_SCHED_.

You can select the data structure used to create a ready queue (CONFIG_SCHED_ALGORITHM):
1. CONFIG_SCHED_DUMB creates queue as a linked list. This is the default option, small, fast and simple.
2. CONFIG_SCHED_SCALABLE creates queue as a red-black tree - this is useful, if you have many runable threas (20 and more).
3. CONFIG_SCHED_MULTIQ creates queue as a multi-level queue. This is original Zephyr scheduling algorithm, but requires a lot of RAM and is incompatible with deadline scheduling.
