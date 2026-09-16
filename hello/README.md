# Hello world from Zephyr!

## Hello Lab goals:

The Lab is split into 2 main Tasks:

1. TASK1 will guide you through the installation process to get all the tools working and configured properly for the labs.
2. TASK2 demonstrates how you can add your application to Zephyr.

## TASK1: Install Zephyr and get it all working

If you're on a classroom computer or in GitHub Codespaces, everything below
is already installed and set up for you — read through this task to
understand what's there, but you don't need to install anything yourself.
If you're setting this up on your own machine, follow the links below.

### Get the Zephyr sources and SDK

1. Go to the [Getting started guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html) on the Zephyr web page.
2. Install [Zephyr and all the dependencies](https://docs.zephyrproject.org/latest/develop/getting_started/index.html#get-zephyr-and-install-python-dependencies).
3. Install the [Zephyr SDK](https://docs.zephyrproject.org/latest/develop/getting_started/index.html#install-zephyr-sdk).

Note: We will work with the qemu_x86 or qemu_cortex_m0 machine for initial emulation. Therefore we are skipping the blink LED which is referred to in the installation
manuals linked above - no LEDs are available in qemu.

### West tool

Read at least the [basic description](https://docs.zephyrproject.org/latest/develop/west/basics.html) to understand what the West meta-tool is.
This documentation explains the structure of the folders and files, and it is very helpful at the beginning.

## TASK2: Hello world application

1. Read about Zephyr applications and their files and folders: <https://docs.zephyrproject.org/latest/develop/application/index.html>.
2. You do not have to create a new application - everything is ready in this folder (done by hand - the source is in the ./src directory).
3. Make sure your virtual environment is active: `source ~/zephyrproject/.venv/bin/activate` (already active for you in GitHub Codespaces - no need to run this there).
4. Compile the application for the qemu_x86 machine using the following command: `zephyr-os-labs$ west build -b qemu_x86 hello/ -t run`.

   A note on that `hello/` argument, since it will come up in every lab:
   `west build` takes the application directory as an optional argument. If
   you run the command from the `zephyr-os-labs` directory (as assumed
   throughout these labs), you need to give it - here, `hello/`. If you `cd`
   into the app's own directory first (e.g. `cd hello`), you can leave it out
   and just run `west build -b qemu_x86 -t run`. Both do the same thing.
5. The command builds the code and starts the qemu emulator in the terminal. You shall see the output below. You can stop qemu with Ctrl+A, X.

You shall see output similar to the following in the terminal (the exact
Zephyr build version and SeaBIOS date will differ - that's expected, it
just reflects whichever Zephyr release you have installed):

```shell
-- west build: running target run
[0/1] To exit from QEMU enter: 'CTRL+a, x'[QEMU] CPU: qemu32,+nx,+pae
SeaBIOS (version ...)
Booting from ROM..
*** Booting Zephyr OS build v4.2.0 ***
Hello world!
```

If you want, you can repeat it for the other machine (qemu_cortex_m0) - just call the build command with `-b qemu_cortex_m0`. The qemu output shall be
similar to what you saw at first.

The different machine has a different RAM, ROM and Flash layout. In the qemu output, you can notice that the memory layout reported by the emulator is different.

### Troubleshooting

If you are switching the boards and you get some errors during the build, you may force west to create the build directory from scratch by adding a parameter:

```shell
west build -p always -b qemu_x86 hello/ -t run
```

### Walk through the files and folders

* Folder `./src` contains all the sources (just `main.c`) in our simple example.
* File `./CMakeLists.txt` contains the information for the compilation - it defines the Zephyr modules included in the project and sets the path to the project sources.
* File `./prj.conf` is dedicated to Zephyr configuration. No config options are set here yet.
* File `README.md` is this document.
* File `sample.yaml` contains the sample definition and links to tests and the Twister tool.
