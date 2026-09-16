# Hello world from Zephyr!

## Hello Lab goals:

The Lab is splitted into 2 main Tasks:

1. TASK1 will guide you through the installation process to get all the tools working and configured properly for the labs.
2. TASK2 demonstrates how you can add your application to the Zephyr.

## TASK1: Install Zephyr and get it all working

### Get the Zephyr sources and SDK

1. Go to [Getting started quide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html) on the Zephyr web page.
2. Install [Zephyr and all the dependencies](https://docs.zephyrproject.org/latest/develop/getting_started/index.html#get-zephyr-and-install-python-dependencies).
3. Install [Zephyr SDK](https://docs.zephyrproject.org/latest/develop/getting_started/index.html#install-zephyr-sdk).

Note: We will work with the qemu_x86 or qemu_m0 machine for initial emulation. Therefore we are skipping the blink LED which is referred in the installation
manuals linked above - no LEDs are available in qemu.

### West tool

Read at least the [basic description](https://docs.zephyrproject.org/latest/develop/west/basics.html) to understand, what the West meta-tool is.
This documentation explains you the structure of the folders and files and it is very helpful at the beginning.

## TASK2: Hello world application

1. Read about Zephyr applications and its files and folders - <https://docs.zephyrproject.org/latest/develop/application/index.html>.
2. You do not have to create a new application, here in this folder is everything ready (done by hand - the source is in the ./src directory).
3. Make sure your virtual environment is ready: `source ~/zephyrproject/.venv/bin/activate`
4. Compile the application for the qemu_x86 machine using following command: `zephyr-os-labs$ west build -b qemu_x86 hello/ -t run`.
5. Command builds the code and starts the qemu emulator in the terminal. You shall see the output. You can stop the qemu by Ctrl+C.

You shall see following output in the terminal.

```shell
-- west build: running target run
[0/1] To exit from QEMU enter: 'CTRL+a, x'[QEMU] CPU: qemu32,+nx,+pae
SeaBIOS (version zephyr-v1.0.0-0-g31d4e0e-dirty-20200714_234759-fv-az50-zephyr)
Booting from ROM..
*** Booting Zephyr OS build zephyr-v3.5.0-575-gf7f800c95749 ***
Hello world!
```

If you want, you can repeat it for the other machine (qemu_m0) - just call the build command with the `-b qemu_m0`. The qemu output shall be
similar to what you have seen at first.

The different machine has different RAM, ROM and Flash layout. In the qemu output, you can notice that the memory layout reported by the emulator is different.

### Troubleshooting

If you are switching the boards and you get some errors during the build, you may force the west to create the build directory from scratch by adding a parameter:

```shell
west build -p always -b qemu_x86 hello/ -t run
```

### Walk through the files and folders

* Folder `./src` contains all the sources (just `main.c`) in our simple example.
* File `./CMakeLists.txt` contains the information for the compilation - it defines the modules of Zephyr included in projects and sets the path to project sources.
* File `./prj.conf` is dedicated to Zephyr configuration. No config options here yet available.
* File `README.md` is this document.
* File `sample.yaml` contains the sample definition and links to tests and the Twister tool.
