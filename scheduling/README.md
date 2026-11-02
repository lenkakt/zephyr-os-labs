# Scheduling

## Scheduling Lab goals:

1. To see how a scheduler divides the CPU between threads.
2. To compare running jobs to completion with time slicing.
3. To see what priorities do, and what they can do to low-priority jobs.
4. To see the difference between preemptive and cooperative threads.

## Before you start

Make sure you are in the `zephyr-os-labs` directory with the virtual environment activated.

Build the example from scratch and check that it works:

```shell
west build -p always -b qemu_cortex_m3 -t run scheduling
```

Use `qemu_cortex_m3` for this lab. Its time is simulated, so you get the same result in every run. On `qemu_cortex_m0`, the results change from run to run.

The program ends, but QEMU keeps running. Press Ctrl+A, then X, to quit it.

## Initial note

The program in `src/main.c` runs the jobs from the table `jobs`. Each job is a thread that keeps the CPU busy for `work_ms` milliseconds and writes its letter into a timeline. The timeline is printed at the end, so printing does not disturb the scheduling. One character is 10 ms:

```
0         100       200       300       400       500
AAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBCCCCCC
```

The table below it shows the **turnaround** of each job: the time from its arrival until it is finished. The system clock ticks every 10 ms, so the times can differ by about 10 ms from what you calculate. This is expected.

In this lab, you change only the table `jobs` and the file `prj.conf`. **Before each run, draw the timeline you expect on paper.** Then compare it with the result and explain any difference.

## TASK1: Run to completion or take turns?

### TASK1.1: No time slicing

Read `src/main.c` and `prj.conf`. All three jobs have the same priority, and `CONFIG_TIMESLICE_SIZE=0` turns time slicing off. Run the program. In what order do the jobs run? How long do B and C wait for A, even though they need much less CPU time?

This method is called First Come, First Served (FCFS).

### TASK1.2: Time slicing

In `prj.conf`, set `CONFIG_TIMESLICE_SIZE=100` and run the program. Then try `CONFIG_TIMESLICE_SIZE=20`. This method is called Round Robin.

Write down the turnaround of each job and the average for all three settings. Who gains, and who does not? Why does A finish last every time?

Hint: a real system also spends time on every thread switch. What would happen with a very short time slice?

## TASK2: Priorities

In Zephyr, a lower number means a higher priority.

### TASK2.1: Shortest job first

Set `CONFIG_TIMESLICE_SIZE=0` and give the shortest job the highest priority:

```c
{ 'A', 7, 0, 300 },
{ 'B', 6, 0, 100 },
{ 'C', 5, 0, 50 },
```

Compare the average turnaround with TASK1. Why is it better? Why can a real operating system rarely schedule like this?

### TASK2.2: Low priority can wait for a long time

Now short jobs with a high priority keep arriving:

```c
{ 'A', 7, 0, 100 },
{ 'C', 5, 0, 40 },
{ 'D', 5, 40, 40 },
{ 'E', 5, 80, 40 },
{ 'F', 5, 120, 40 },
{ 'G', 5, 160, 40 },
```

When does A get the CPU? How long does it take to finish 100 ms of work? What would happen if the short jobs never stopped arriving? This problem is called **starvation**.

## TASK3: Preemptive or cooperative?

### TASK3.1: Preemptive threads

A long job with a low priority is running when an important job H arrives:

```c
{ 'A', 7, 0, 300 },
{ 'H', 5, 100, 50 },
```

When does H start? What happens to A at that moment?

### TASK3.2: Cooperative threads

A thread with a negative priority is **cooperative**: nobody can take the CPU from it. Change only the priorities. H still has the higher priority:

```c
{ 'A', -1, 0, 300 },
{ 'H', -2, 100, 50 },
```

When does H start now? Compare its turnaround with TASK3.1. Would you use a cooperative thread for a long computation? Where can it be useful?
