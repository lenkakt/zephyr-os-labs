# The device management

In this lab, you shall learn how to manage devices in Zephyr OS.
You will learn how to create a device object, add resources to it, and manage the device object.
We will spent some time with device trees.

## Before you start

Make sure you're in the `zephyr-os-labs` directory with the virtual environment activated.

Build the example, clean all the old stuff and ensure that it is working:

```shell
west build -p always -b qemu_cortex_m0 -t run ./devices
```

## TASK1 Devices introduction

### TASK1.1 Shell

One of the most useful reources in the Zephyr OS is its shell. It allows
you to interact with the system in runtime, check variables, change settings, etc.
In this task, you will create a simple application that uses the shell.

It is not a Bash/sh shell like in Linux or other POSIX systems, but a
simple command line interface that you can use over UART.

It was designed to help you with debugging and interacting with the system.
Check the documentation here to understand the basics: https://docs.zephyrproject.org/latest/reference/shell/index.html

To switch on the shell, you do not need to write any code, just enable it in the configuration file prj.conf.
You need to add the following lines to the prj.conf file:

```
CONFIG_SHELL=y
CONFIG_SHELL_BACKEND_SERIAL=y
CONFIG_UART_CONSOLE=y
```

The "CONFIG_SHELL=y" enables the shell, the second line specifies that the shell will use the serial backend (UART),
and the third line enables the UART console.

Your first task is to create a void main function and just enable shell in the configuration and list, for example,
the running treads by using the "threads" command:

```shell
west build -p always -b qemu_cortex_m0 -t run ./devices
# After the build, when emulation starts, it shall show you the shell prompt.
# Investigate and find out how to list the running threads.
```

### TASK 1.2: Your own shell command

It is very easy to create your own shell commands. It is actually similar to creating threads or processing interupts.

You need to do 2 things:

1. Create a function that will be called when the command is executed.
   The function must have the following signature:

```c
int cmd_name(const struct shell *shell, size_t argc, char **argv)
```

2. Register the command using the SHELL_CMD_REGISTER macro.
3. The macro must be called outside of any function, for example, after the function definition.

See the documentation for details.
Your task here is to create a command with your name, for example, "my_cmd" that prints
"Hello, I am the command!" or similar when executed.

### TASK 1.3: Shell and threads

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

## TASK2 Device trees

The device tree is a data structure  that describes the hardware components of a system.
It is used by the Zephyr operating system to understand the hardware it is running on. And it is the key
part of the Zephyr OS device model. It helps to abstract the hardware details from the software,
making it easier to write portable code.

To understand device trees, start with the documentation as usual: https://docs.zephyrproject.org/latest/guides/dts/index.html
The documentation is quite extensive, so focus on the introduction and basic concepts.

### TASK2.1: Exploring device tree

In this task, you will explore the device tree of your Zephyr OS application.
You can find the device tree files in the build/zephyr/zephyr.dts file after you build the - it can be just the initial
"hello world" application.

Your task is to find the following information in the device tree:
1. The CPU architecture and model.
2. The memory layout (RAM and Flash).
3. The UART device used for the console.
4. Any GPIO devices present.
5. The interrupt controller used.
6. Any timers present.

The device tree is just a text file, so you can read it in any text editor.
As you see, the main device tree file is created from different sources, so in the final file you get also
links to the original files.

### TASK2.2: Browsing the device tree in the shell

In the configuration, switch on the shell and add the device shell using:

```
CONFIG_SHELL=y
CONFIG_SHELL_BACKEND_SERIAL=y
CONFIG_DEVICE_SHELL=y
CONFIG_UART_CONSOLE=y
```

Rebuild and run the application. Test the device command and check the outputs.

### TASK2.3: Modyfying the device tree

We will now add a "dummy" device to the device tree and add it to the main device tree using an overlay file.
First, create a new subdrectory called "boards" inside the "devices" directory.

Then create a device tree fragment that we want to add to the main device tree. To inform the build
system that we want to add this to main device tree. we need to name it after a board and add the
"overlay" suffix. So, when working with the qemu_cortex_m0 board, we need to create a file called
"qemu_cortex_m0.overlay" inside the "boards" directory.

Define a "dummy" device here:

```dts
/ {
  dummy0: dummy {
    compatible = "tul,dummy";
    foo = <1234>;
    status = "okay";
    label = "DUMMY0";
  };
};
```

As a next step, we need to create a binding for our dummy device. It will be stored in the "dts/bindings/misc"
directory in the file with the same name as the compatible string: "tul,dummy.yaml".

It shall contain the definition of the properties we used in the device tree fragment:

```yaml
# dts/bindings/misc/tul,dummy.yaml
compatible: "tul,dummy"
description: Minimal dummy device for demo
properties:
  foo:
    type: int
    required: true
  label:
    type: string
    required: false
```

The last thing we need to add is the definition of our "vendor" the tul (we used this, because the labs were created
at Technical University of Liberec).

In the "dts/bindings" create a file called vendor-prefixes.txt with following content:

```text
tul  Technical University of Liberec
```

Compile and run the application. If you test the main device tree, you shall see your dummy device at the end.
But if you list the devices using the shell command, you will not see it there. Why? The driver
which initializes the device is missing.

In the solutions, you can check all the files (you have there boards directory and the dts directory as well).

### TASK2.4: Adding the device driver

To see the device listed in the shell, we must create a driver for it. The driver is a simple C file
that initializes the device and registers it with the device model.

Create a new source file called "tul_dummy.c" in the "src" directory. You have an inspiration in the
solutions directory. Do not forget to list the new source file in the CMakeLists.txt:

```cmake
target_sources(app PRIVATE
    src/main.c
    src/tul_dummy.c)
```

The driver turns a Devicetree node (compatible = "tul,dummy") into a runtime device by defining
a small config struct, an init() function, and then instantiating it with DEVICE_DT_DEFINE/DEVICE_DT_INST_DEFINE.

At build time, DT macros (e.g., DT_PROP, DT_INST(...)) pull properties like foo from
the node, and Zephyr's init system calls your init() so the device becomes READY and shows up in device list.

When you finish, rebuild and run the application. Now you should see your dummy device in the device list.
