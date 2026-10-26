# Synchronization

## Synchronization Lab goals:

1. To see a race condition and understand why it happens.
2. To protect shared data with atomic operations and mutexes.
3. To use a semaphore for a limited resource and as a signal, and a condition variable to wait for a condition.
4. To observe priority inversion and how priority inheritance solves it.
5. To create a deadlock and remove it by lock ordering.
6. To share data with an interrupt handler safely.
7. To compare what the different kinds of protection cost.

## Before you start

Make sure you are in the `zephyr-os-labs` directory with the virtual environment activated.

Build the example from scratch and check that it works:

```shell
west build -p always -b qemu_cortex_m0 -t run synchro
```

Use qemu_cortex_m0 or any other Arm Cortex-M board for this lab.

## Initial note

All tasks in this lab take place in a bank. Most of them use one story: a mother sends money to her daughter's bank account, and the daughter withdraws it. Each of them is a thread, and the account balance is the data they share.

TASK1 to TASK5 and Putting it all together are meant for the lab. TASK3.2, TASK3.3 and TASK6 are homework.

In TASK1, you only run and change the prepared program in `src/main.c`. From TASK2 on, you write your own programs – replace `src/main.c` with your code, or create a new application for each task. Some solutions contain a `#define` switch (for example `USE_MUTEX`) that turns one version of the program into another; the comment next to it says what it does.

The programs end, but QEMU keeps running. Press Ctrl+A, then X, to quit it.

As in the previous labs, try to write the code yourself before you look at the solutions.

## TASK1: Race condition

### TASK1.1: Where did the money go?

Read `src/main.c`. The mother deposits 100 CZK a thousand times, and the daughter withdraws 100 CZK a thousand times, so the final balance should be 0 CZK. Build and run the program.

Explain the result. Look at the function `change_balance`: the balance is read, then the new value is written. What happens if the other thread runs between these two steps? Draw the first few steps of both threads on paper.

This situation is called a **race condition**: the result depends on the timing of the threads. The code between reading and writing the shared data is a **critical section** – only one thread may be inside at a time.

### TASK1.2: A hidden bug

In `src/main.c`, set `SIMULATE_INTERRUPT` to 0 and run the program again. The balance is now correct. Is the code correct, too?

Hint: `k_yield()` only makes the thread switch happen every time. Without it, a thread switch in the critical section is rare – but on a real device with interrupts, or on a processor with more cores, it does happen. A race condition that appears only once a week is much harder to find than one that appears every time.

Set `SIMULATE_INTERRUPT` back to 1 for the next tasks.

## TASK2: Protecting the balance

### TASK2.1: Atomic operations

The simplest protection for a single variable is an **atomic operation**: the processor reads, changes and writes the value in one step that cannot be interrupted. Read the [Atomic services documentation](https://docs.zephyrproject.org/latest/kernel/services/other/atomic.html).

Change `balance` to the type `atomic_t` and rewrite `change_balance` with `atomic_add`. Keep `k_yield()` in the function. Is the balance correct now?

Optional: how does the processor do it? Build the program with a disassembly listing for two boards:

```shell
west build -p always -b qemu_cortex_m3 synchro -- -DCONFIG_OUTPUT_DISASSEMBLY=y
```

Find `change_balance` in `build/zephyr/zephyr.lst`. Cortex-M3 uses the instructions `ldrex` and `strex`: the write succeeds only if nobody else touched the variable in the meantime, otherwise the loop tries again. Now build for `qemu_cortex_m0` and look for `z_impl_atomic_add`. Cortex-M0 has no such instructions – what does Zephyr do instead? (Hint: look up `cpsid i`.) Would this work on a processor with more cores?

### TASK2.2: Warm-up: waiting for a mutex

An atomic operation protects one variable. When a critical section is longer, you need a lock. Read the [Mutexes documentation](https://docs.zephyrproject.org/latest/kernel/services/synchronization/mutexes.html).

Write a new short program: the mother and the daughter visit the account three times each. At each visit, they try to lock a shared mutex with `k_mutex_lock(&account_mutex, K_NO_WAIT)`. If they get the mutex, they print a message, hold it for a while (the mother 2 s, the daughter 0.5 s) and unlock it. If the account is busy, they print a message and come back later. Print the time (`k_uptime_get()`) in every message.

Then change `K_NO_WAIT` to `K_FOREVER` and compare the outputs. What is the difference between giving up and waiting? What is the thread doing while it waits?

Note: holding a lock for seconds is a bad habit – a critical section should be as short as possible. Here it is long on purpose, so that you can see the waiting.

### TASK2.3: The balance protected by a mutex

Go back to the bank account from TASK1. Protect the critical section in `change_balance` with a mutex. Keep `k_yield()` inside. Is the balance correct? What happens when the thread holding the mutex calls `k_yield()`?

## TASK3: Semaphores

Read the [Semaphores documentation](https://docs.zephyrproject.org/latest/kernel/services/synchronization/semaphores.html) first.

### TASK3.1: Bank counters

The bank has 3 counters and 8 customers. A **counting semaphore** is a good tool for this: its value is the number of free counters. `k_sem_take` lets a customer in, or makes the customer wait when the value is 0; `k_sem_give` frees one counter.

Each customer is a thread; all threads share one entry point function with the customer number as a parameter. Create them with `k_thread_create` in a loop.

The semaphore only says *how many* counters are free, not *which ones*. Keep the numbers of free counters in a data structure – a stack is a good choice. All customers use the stack, so protect it with a mutex.

Each customer visits the bank twice. At each visit:

1. The customer waits for the semaphore.
2. The customer takes a free counter number from the stack, prints it, and is served for a while (`k_sleep`).
3. The customer returns the counter number to the stack, and only then gives the semaphore. The customer leaves the bank and comes back later.

Print the time in every message. How many customers are served at the same time? What would happen if the customer gave the semaphore before returning the counter to the stack?

### TASK3.2 (homework): A semaphore as a signal

A semaphore does not have to count resources – it can also deliver a signal "done, you can go on". Create a semaphore with the initial value 0. The mother sends 1000 CZK three times, every 2 seconds, and after each payment she gives the semaphore. The daughter waits for the semaphore (`k_sem_take` with `K_FOREVER`) and then withdraws the money. Use `atomic_t` for the balance, as in TASK2.1.

Print the time in every message. What does the daughter do between the payments? Who gives the semaphore and who takes it – could you do the same with a mutex?

Zephyr also has [events](https://docs.zephyrproject.org/latest/kernel/services/synchronization/events.html) (`k_event`) for signalling between threads; look at them if you want to wait for more different signals at once.

### TASK3.3 (homework): Waiting for enough money

The daughter needs 400 CZK at once, but the mother sends only 100 CZK every second. The daughter has to wait until the balance is high enough – and a semaphore does not fit here, because she waits for a **condition** on the shared data. Read the [Condition variables documentation](https://docs.zephyrproject.org/latest/kernel/services/synchronization/condvar.html).

Protect the balance with a mutex. The daughter locks the mutex and, while the balance is too low, waits with `k_condvar_wait`. The mother locks the mutex, deposits the money, calls `k_condvar_signal` and unlocks the mutex.

Why does the daughter check the condition in a `while` loop and not with `if`? What does `k_condvar_wait` do with the mutex while the daughter waits?

## TASK4: Priority inversion

A higher priority thread always runs first – or does it? In this task, three threads work in the bank:

- *statement* (low priority): prepares a monthly statement and locks the account for 100 ms,
- *backup* (medium priority): makes a backup for 500 ms; it needs the processor, not the account,
- *payment* (high priority): a card payment that needs the account and has to be fast.

### TASK4.1: The payment waits too long

Write the program. Protect the account with a semaphore with the initial value 1 (`K_SEM_DEFINE(account_sem, 1, 1)`). The statement starts first and locks the account. The payment comes 30 ms later, the backup 40 ms later (use `k_msleep` at the start of the thread). Simulate the work with `k_busy_wait` – unlike `k_sleep`, it keeps the processor busy. Print how long the payment waited for the account.

Explain the output. Why does the high priority payment wait for the medium priority backup, although the backup does not use the account at all? This is called **priority inversion**. A famous case happened on the Mars Pathfinder probe in 1997 – look it up.

### TASK4.2: Priority inheritance

Replace the semaphore with a mutex and run the program again. How long does the payment wait now, and in which order do the threads finish?

Read about [priority inheritance](https://docs.zephyrproject.org/latest/kernel/services/synchronization/mutexes.html#priority-inheritance) in the mutex documentation. Why can a mutex do this, but a semaphore cannot? (Hint: who owns a semaphore?)

## TASK5: Deadlock

### TASK5.1: Two transfers

Each account has its own mutex. A transfer locks both accounts – the one it takes money from and the one it sends money to – changes both balances and unlocks them.

Write the program: the mother sends 1 CZK to the daughter a thousand times, and the daughter sends 1 CZK back a thousand times, at the same time. Each transfer locks the "from" account first, then the "to" account. Put `k_yield()` between the two locks, as in TASK1. In `main`, wait for the threads with a timeout (`k_thread_join(tid, K_SECONDS(3))`) and print a warning if they do not finish.

What happened? Draw both threads and both mutexes on paper: who holds what, and who waits for what? This is a **deadlock**: each thread holds one lock and waits for the other one, forever.

A deadlock can happen only when four conditions hold at the same time: mutual exclusion, hold and wait, no preemption, and circular wait. Which of them can you break here?

Fix the program: lock the accounts always in the same order – the account with the lower number first, whatever the direction of the transfer. Why can the circle of waiting threads not close now?

### TASK5.2: The dining philosophers

Five philosophers sit at a round table. There is one fork between each two of them, five forks in total, and a philosopher needs both neighbouring forks to eat.

Write the program: each philosopher is a thread, each fork is a mutex. A philosopher takes the left fork, thinks for 10 ms, takes the right fork, eats, puts both forks down and thinks again – three meals in total. Detect a deadlock in `main` the same way as in TASK5.1.

Then fix it with the rule from TASK5.1: number the forks and let every philosopher take the fork with the lower number first. Which philosopher now takes the forks in a different order than the others?

Zephyr has its own, more elaborate version of this problem: see the [philosophers sample](https://docs.zephyrproject.org/latest/samples/philosophers/README.html).

## TASK6 (homework): Sharing data with an interrupt

The mother now pays regularly: a [timer](https://docs.zephyrproject.org/latest/kernel/services/timing/timers.html) (`k_timer`) sends 100 CZK to the account every 10 ms. The timer function runs in an **interrupt**, not in a thread. In `main`, the daughter withdraws 100 CZK a hundred times; each withdrawal reads the balance, is checked by the bank for 2 ms (`k_busy_wait(2000)`), and writes the new balance. At the end, stop the timer, count the payments and compare the balance with the expected value.

### TASK6.1: Lost payments

Run the program without any protection. How many payments were lost, and why? Note that there is no `k_yield()` now – what interrupts the daughter?

### TASK6.2: A mutex in an interrupt?

Protect the balance with a mutex in both places. Build the program with assertions enabled:

```shell
west build -p always -b qemu_cortex_m0 -t run synchro -- -DCONFIG_ASSERT=y
```

What happens? An interrupt handler must never wait – it is not a thread, so the scheduler cannot put it to sleep. It may give a semaphore (`k_sem_give`), but it must not take a mutex.

### TASK6.3: A spinlock

Protect the balance with a [spinlock](https://docs.zephyrproject.org/latest/kernel/services/synchronization/spinlocks.html) (`k_spin_lock` and `k_spin_unlock`) in both places. Is the balance correct now? On a single-core processor, a spinlock does not spin at all – what does it do instead? (Hint: what did you see for `atomic_add` on Cortex-M0 in TASK2.1?) Why should the code between `k_spin_lock` and `k_spin_unlock` be as short as possible?

## Putting it all together

You have protected the balance in several ways: with an atomic operation, a mutex and a spinlock. They all give the correct result, but they do not cost the same. Find out the order yourself.

Write a program that changes the balance 100 000 times in one thread – four times in a row: without protection, with `atomic_add`, with `k_spin_lock` and with `k_mutex`. Measure each loop with `k_cycle_get_32()` and convert the result with `k_cyc_to_us_floor64()`. Declare the unprotected balance `volatile` – try it without `volatile` too, and explain the result.

Build for `qemu_cortex_m3`. On this board, QEMU runs in *icount* mode: every instruction takes the same virtual time, so the results are the same in every run. They count instructions rather than real time.

Based on what you measured, write down:

1. the order of the four versions from the cheapest to the most expensive, and how many times more expensive the mutex is than no protection;
2. why the mutex is the most expensive, although no other thread waits for it in this program (what does `k_mutex_lock` have to check and remember?);
3. what changes on `qemu_cortex_m0` – which two versions swap places, and why (remember TASK2.1). Note that `qemu_cortex_m0` does not use icount: the times come from the clock of your computer and change from run to run;
4. when it is still better to use a mutex than a spinlock, although the mutex is more expensive (remember TASK4 and TASK6).
