*********************
The device management
*********************

In this lab, you shall learn how to manage devices in Zephyr OS.
You will learn how to create a device object, add resources to it, and manage the device object.
We will spent some time with device trees.

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

    west build -p always -b qemu_cortex_m0 -t run ./devices

==========================
TASK1 Devices introduction
==========================
--------------
TASK1.1 Shell
--------------  
One of the most useful reources in the Zephyr OS is its shell. It allows 
you to interact with the system in runtime, check variables, change settings, etc.
In this task, you will create a simple application that uses the shell.

It is not a Bash/sh shell like in Linux or other POSIX systems, but a 
simple command line interface that you can use over UART. 

It was designed to help you with debugging and interacting with the system. 
Check the documentation here to understand the basics: https://docs.zephyrproject.org/latest/reference/shell/index.html

To switch on the shell, you do not need to write any code, just enable it in the configuration file prj.conf.
You need to add the following lines to the prj.conf file:   

.. code-block:: config

    CONFIG_SHELL=y
    CONFIG_SHELL_BACKEND_SERIAL=y
    CONFIG_UART_CONSOLE=y

The "CONFIG_SHELL=y" enables the shell, the second line specifies that the shell will use the serial backend (UART), 
and the third line enables the UART console.

Your first task is to create a void main function and just enable shell in the configuration and list, for example,
the running treads by using the "threads" command:

.. code-block:: shell

    west build -p always -b qemu_cortex_m0 -t run ./devices
    # After the build, when emulation starts, it shall show you the shell prompt.
    # Investigate and find out how to list the running threads.

---------------------------------
TASK 1.2: Your own shell command
---------------------------------

It is very easy to create your own shell commands. It is actually similar to creating threads or processing interupts.

You need to do 2 things:

1. Create a function that will be called when the command is executed.
   The function must have the following signature:

.. code-block:: c

    int cmd_name(const struct shell *shell, size_t argc, char **argv)  

2. Register the command using the SHELL_CMD_REGISTER macro.
3. The macro must be called outside of any function, for example, after the function definition.

See the documentation for details.
Your task here is to create a command with your name, for example, "my_cmd" that prints 
"Hello, I am the command!" or similar when executed.

--------------------------------
TASK 1.3: Shell and threads
--------------------------------

As you see, the shell takes all the serial console, so if you have any printk() in your code,
you will not see it, because the shell is using the same UART.

To demonstrate this, create several threads that print their name and a counter value in a loop with some delay.
Verify that you do not see any output from the threads when the shell is active; if you used printf or printk to 
print the message.

To print out to shell, you need to use the function shell_print() instead of printk(). And this function
requires a pointer to the shell instance as the first parameter.

To get the shell instance, you can use the shell_backend_uart_get_ptr() function (you need to include shell/shell_uart.h header file).  

So, modify your threads to print their messages using shell_print() instead of printk().

So you see the output in shell now? If yes, you are done with this task.


===================
TASK1.2 Device Tree
===================