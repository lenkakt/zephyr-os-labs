# Devices

## Devices Lab goals:

1. To read the description of the board hardware in the device tree.
2. To list the devices the system created and find out why some nodes of the device tree are not among them.
3. To use a driver from an application without knowing the addresses of the device registers.
4. To add your own device to the device tree and write a simple driver for it.

## Before you start

Make sure you are in the `zephyr-os-labs` directory with the virtual environment activated.

Build the example from scratch and check that it works:

```shell
west build -p always -b qemu_cortex_m0 -t run devices
```

You should see:

```
*** Booting Zephyr OS build v4.4.2 ***
Hello from the devices lab!
```

The program ends, but QEMU keeps running. Press Ctrl+A, then X, to quit it.

## Initial note

All tasks use the board `qemu_cortex_m0`. QEMU emulates the chip nRF51822 with a Cortex-M0 processor, so you do not need any hardware.

The build may print a warning about `QEMU_ICOUNT_SHIFT`. You can ignore it.

Some tasks need configuration options. Add them to `devices/prj.conf`, one option per line. Always build with `-p always` when you change `prj.conf` or any device tree file.

The tasks give you templates. Replace the content of the file with the template and complete the lines marked `TODO`. As in the previous labs, try to complete them yourself before you look at the solutions in `solutions/`.

## Required and extension tasks

All tasks in this lab are required.

## How Zephyr knows its hardware

Devices such as a serial port (UART), GPIO pins or timers sit at fixed addresses in the chip. They cannot tell the system what they are. The system learns about them from a **device tree**: a text file that describes the hardware of the board.

The device tree is a tree of **nodes**. Each node describes one part of the hardware and has **properties**, pairs of a name and a value. The most important properties are:

- `compatible`: what kind of hardware it is, in the form `"vendor,model"`. The system uses it to choose the driver.
- `reg`: the address of the registers and the size of the register area.
- `interrupts`: the number of the interrupt the device uses.
- `status`: `"okay"` if the device is switched on, `"disabled"` if it is not.

Zephyr reads the device tree when you build the program, not when it runs. For each node that is switched on and has a driver, the build creates a `struct device` in your program. Your application gets this structure and calls the functions of the driver through it.

The documentation of the device tree in Zephyr: https://docs.zephyrproject.org/latest/build/dts/index.html

## TASK1: The hardware description (required)

After every build, the complete device tree of the board is in the file `build/zephyr/zephyr.dts`. It is put together from several files: a file for the chip, a file for the board and, later in this lab, your own file.

Build the example as in "Before you start", quit QEMU, and print the node of the first serial port:

```shell
grep -n -A8 "uart0: uart@" build/zephyr/zephyr.dts
```

You should see these lines (the line numbers may differ):

```
uart0: uart@40002000 {
        compatible = "nordic,nrf-uart"; /* in zephyr/dts/arm/nordic/nrf51822.dtsi:102 */
        reg = < 0x40002000 0x1000 >;    /* in zephyr/dts/arm/nordic/nrf51822.dtsi:103 */
        interrupts = < 0x2 0x1 >;       /* in zephyr/dts/arm/nordic/nrf51822.dtsi:104 */
        status = "okay";                /* in zephyr/boards/qemu/cortex_m0/qemu_cortex_m0.dts:40 */
        current-speed = < 0x1c200 >;    /* in zephyr/boards/qemu/cortex_m0/qemu_cortex_m0.dts:41 */
```

The comment at the end of each line tells you which file the property comes from. Answer these questions:

1. Which vendor made this serial port and which driver will the system choose for it?
2. At which address do its registers start and how many bytes do they take?
3. Which interrupt does it use? (The first number is the interrupt, the second its priority.)
4. The property `status` comes from the board file, not from the chip file. Why does the board decide whether the serial port is switched on? Hint: the chip has the serial port inside, but is it always connected to something on every board?
5. `current-speed` is `0x1c200`. What speed is it in decimal?

The device tree also says which serial port the board uses for the console. Print the node `chosen`:

```shell
grep -n -A8 "chosen {" build/zephyr/zephyr.dts
```

6. Find the line `zephyr,console`. Which node does the console use?

## TASK2: Devices in a running system (required)

Zephyr has a shell: a simple command line you can use over the serial port. One of its commands lists the devices the system created.

Add these lines to `devices/prj.conf`:

```
CONFIG_SHELL=y
CONFIG_DEVICE_SHELL=y
```

Replace the content of `src/main.c` with this program. There is nothing to complete, the shell does the work:

```c
#include <zephyr/kernel.h>

int main(void)
{
	printk("Type 'device list' and press Enter.\n");
	return 0;
}
```

Build and run it. When you see the prompt `uart:~$`, type `device list` and press Enter. You should see:

```
uart:~$ device list
devices:
- clock@40000000 (READY)
  DT node labels: clock
- gpio@50000000 (READY)
  DT node labels: gpio0
- uart@40002000 (READY)
  DT node labels: uart0
```

`READY` means that the driver initialized the device successfully.

Quit QEMU and count the nodes that are switched on in the device tree:

```shell
grep -c 'status = "okay"' build/zephyr/zephyr.dts
```

1. The device tree has 24 nodes with `status = "okay"`, but the system created only three devices. Why? Hint: a node becomes a device only if there is a driver for it in the program. Which drivers did the configuration include?
2. Find `uart0` in the list. How does the name `uart@40002000` relate to the node from TASK1?

## TASK3: Using a driver (required)

An application should not write to the registers of a device itself. It asks the driver to do it. In Zephyr, the application gets the device from the device tree and calls the functions of its class. All functions for serial ports start with `uart_`, for example `uart_poll_out()`, which sends one character.

Remove the two lines with `SHELL` from `devices/prj.conf`. The shell would use the same serial port and mix its prompt into the output.

Replace the content of `src/main.c` with this template and complete the three lines marked `TODO`:

```c
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>

/* The device tree node with the label uart0 */
#define UART_NODE DT_NODELABEL(uart0)

/* TODO: get the device for UART_NODE with DEVICE_DT_GET() instead of NULL */
static const struct device *const uart = NULL;

int main(void)
{
	/* TODO: print the address of the uart0 registers with DT_REG_ADDR(UART_NODE) instead of 0 */
	printk("uart0 registers start at 0x%08lx\n", (unsigned long)0);

	if (!device_is_ready(uart)) {
		printk("uart0 is not ready\n");
		return 0;
	}
	printk("uart0 device name: %s\n", uart->name);

	for (const char *c = "Hello from the UART driver!\n"; *c != '\0'; c++) {
		/* TODO: send the character *c with uart_poll_out() */
	}
	return 0;
}
```

If you build the template without changes, it prints:

```
uart0 registers start at 0x00000000
uart0 is not ready
```

When you complete it, you should see:

```
uart0 registers start at 0x40002000
uart0 device name: uart@40002000
Hello from the UART driver!
```

1. Where does the address `0x40002000` come from? Compare it with TASK1.
2. Your program printed the address, but it never used it to send the characters. Who writes to the registers of the serial port?
3. Why does the program call `device_is_ready()` before it uses the device?
4. `uart_poll_out()` waits in a loop until the transmitter of the serial port is free, then it writes the character. This way of waiting is called polling. What does the processor do while it waits?

## TASK4: Your own device and driver (required)

Now you will add a device of your own: a "dummy" device. It has no hardware behind it, only one property, the number `foo`. The driver reads the number from the device tree.

### TASK4.1: A node in the device tree (required)

You do not change the files of the board. You add your node in an **overlay**: a file that the build adds to the device tree of the board. The build finds it by the name of the board.

Add the two lines with `SHELL` back to `devices/prj.conf`, so you can list the devices again. Replace `src/main.c` with the program from TASK2.

Create the directory `devices/boards` and in it the file `qemu_cortex_m0.overlay`:

```dts
/ {
	dummy0: dummy {
		compatible = "tul,dummy";
		foo = <1234>;
		status = "okay";
	};
};
```

The node `dummy` is added to the root `/` of the tree. `dummy0` is its label, the name you use in C code.

Each `compatible` needs a **binding**: a file that says which properties the node has and of which type. Create the directory `devices/dts/bindings/misc` and in it the file `tul,dummy.yaml`:

```yaml
description: Dummy device for the devices lab

compatible: "tul,dummy"

properties:
  foo:
    type: int
    required: true
    description: A number the driver reads from the device tree
```

The first part of `compatible` is the vendor. `tul` is not a known vendor (it stands for the Technical University of Liberec, where the labs were created), so add it. Create the file `devices/dts/bindings/vendor-prefixes.txt` with one line. The vendor prefix and the name are separated by a tab:

```text
tul	Technical University of Liberec
```

Build and run the program. Quit QEMU and check that your node is in the device tree:

```shell
grep -n -A4 "dummy0:" build/zephyr/zephyr.dts
```

You should see your node, with `foo = < 0x4d2 >;` (1234 in hexadecimal). Run the program again and type `device list`.

1. Is your device in the list? Why not? Hint: compare with question 1 of TASK2.

### TASK4.2: The driver (required)

A driver is not a program with its own `main`. It is a set of functions the system calls. Your driver has one function, `dummy_init()`, which the system calls once at startup. The driver also tells the system which node it serves: the macro `DEVICE_DT_DEFINE()` creates the `struct device` for the node.

Create the file `devices/src/tul_dummy.c` with this template and complete the two places marked `TODO`:

```c
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>

/* The device tree node with the label dummy0 */
#define DUMMY_NODE DT_NODELABEL(dummy0)

/* Configuration of the device: values taken from the device tree */
struct dummy_config {
	int foo;
};

/* The kernel calls this function once at startup to initialize the device */
static int dummy_init(const struct device *dev)
{
	const struct dummy_config *cfg = dev->config;

	printk("dummy: init, foo = %d\n", cfg->foo);
	return 0;
}

static const struct dummy_config dummy_config0 = {
	/* TODO: read the property foo of DUMMY_NODE with DT_PROP() instead of 0 */
	.foo = 0,
};

/* TODO: create the device for DUMMY_NODE with DEVICE_DT_DEFINE():
 *   node:            DUMMY_NODE
 *   init function:   dummy_init
 *   power management: NULL
 *   data:            NULL
 *   config:          &dummy_config0
 *   init level:      POST_KERNEL
 *   priority:        CONFIG_KERNEL_INIT_PRIORITY_DEFAULT
 *   API:             NULL
 */
```

Add the new file to `devices/CMakeLists.txt`:

```cmake
target_sources(app PRIVATE
    src/main.c
    src/tul_dummy.c)
```

If you build the template without changes, the compiler warns that `dummy_init` and `dummy_config0` are defined but not used. Nobody calls them yet, because the device does not exist. When you complete it, build and run the program. You should see:

```
dummy: init, foo = 1234
*** Booting Zephyr OS build v4.4.2 ***
Type 'device list' and press Enter.
```

Type `device list`. Your device is at the end of the list:

```
- dummy (READY)
  DT node labels: dummy0
```

1. The line `dummy: init` appears before the line `Booting Zephyr OS`. Why? Hint: when does the system call the init function of a driver?
2. Change `foo` in the overlay to `42`, build and run the program again. Did you change any C code? Where did the driver get the new value?
3. The serial port in TASK1 has `compatible = "nordic,nrf-uart"`. Your node has `compatible = "tul,dummy"`. What do these two strings have in common, and what does the system use them for?
