# Zephyr configurations

In this section, we will focus on all possible parameterizations and configurations that affect the final behaviour of your system.

These are:
You set the resulting system's source code composition and properties with parameters in prj.conf file using the Kconfig syntax.

You set how the system is compiled and assembled from the sub-applications in CMakeLists.txt.

Environment variables also affect the compiler's and other tools' behaviour and can be defined in CMakeLists.txt or as shell variables.

We compose the whole project from different sub-repositories of code using so-called manifests written in YAML.

This exercise focuses on Kconfig (TASK1), CMakeLists.txt (TASK2), and environment variables (TASK3); we'll cover manifests in another section.

## TASK1: prj.conf and Kconfig

Main goal: To understand how the system configuration works, Kconfig and the proper way to set your OS configuration.

The configuration options are macros used in the source code. However, they are not defined here - the generating header file is composed only during compilation from the configuration files. It is called autoconf.h and can be found after successful compilation in zephyr-os-labs/build/zephyr/include/generated/autoconf.h

The good point to start is always the documentation, so read the initial description at <https://docs.zephyrproject.org/latest/build/kconfig/index.html>.

### Before you start

Make sure you're in the `zephyr-os-labs` directory with the virtual environment activated.

Build the example as it is, from scratch (`-p always`, "pristine" - throws
away any previous build output and reconfigures everything; you'll want
this every time you change boards or Kconfig fragments in this lab, since
otherwise west can silently keep using stale, cached configuration), to be
sure it is working:

```shell
west build -p always -b qemu_x86 config/ -t run
```

We will play with these options:

1. CONFIG_BOOT_DELAY <https://docs.zephyrproject.org/2.7.5/reference/kconfig/CONFIG_BOOT_DELAY.html#std-kconfig-CONFIG_BOOT_DELAY>

2. CONFIG_BOOT_BANNER <https://docs.zephyrproject.org/2.7.5/reference/kconfig/CONFIG_BOOT_BANNER.html#std-kconfig-CONFIG_BOOT_BANNER>

3. CONFIG_BOOT_BANNER_STRING

A good idea is to read the documentation related to these options.

### TASK1.1: prj.conf

You can manually write the options into prj.conf.
In the first line, you see the CONFIG_BOOT_DELAY. To get it working, delete the # sign.

CONFIG_BOOT_BANNER and CONFIG_BOOT_BANNER_STRING aren't in prj.conf yet -
add both lines yourself to set up the boot message (see the options
documentation linked in TASK1 description for the exact syntax).

To compile and run the emulator, use:

```shell
west build -b qemu_x86 config/ -t run
```

If you boot successfully, try to play with switching the message on and off.
Now is a perfect time to write any nonsense to prj.conf to find out what will happen during compilation time.

No success? Try the file solutions/task1_1.prj.conf.

### TASK1.2: menuconfig, guiconfig and Kconfig

There are many configuration options, and they depend on each other. Writing a configuration from scratch in a file and considering all the dependencies is challenging; no one asks you to do it!

The right way is to use ready-made tools that group the options into graphical menus. You have two options: if a GUI is available during development, you can use the GUI tool (guiconfig) or the text menus available on the command line (menuconfig).

We use the Kconfig language to set up the menu's structure. You can find Kconfig files in the sources defining the menu structure and options description—the online documentation generated from these files (e.g. here: <https://docs.zephyrproject.org/2.7.5/reference/kconfig/CONFIG_BOOT_BANNER.html#std-kconfig-CONFIG_BOOT_BANNER>).
All the options used in this lab are described in the zephyr-os-labs/zephyr/kernel/Kconfig file.

1. Comment out your settings in prj.conf.

2. Start one of the interfaces and set your menu options. Save them. The entire configuration is now saved in zephyr-os-labs/build/zephyr/config.

To start the menuconfig use: `west build -b qemu_x86 config/ -t menuconfig`

To start the guiconfig use: `west build -b qemu_x86 config/ -t guiconfig`

3. When you finish the settings, save your changes and quit the menu.

4. Recompile the whole project and start it up again.

5. Test what has a higher priority: what will be used if you now set a different banner in the prj.conf file than in the menu?

## TASK2: Compilation, Configuration, build system variables and CMakeLists.txt

The system configuration and compilation process is orchestrated by the CMake.
The general description is given in the Zephyr documentation here: <https://docs.zephyrproject.org/latest/build/cmake/index.html>

The entry point for CMake is the applications's CMakeLists.txt file, stored in the application's main directory.

The initial CMakeLists.txt for this task contains only the application's basic template, specifying the application's name, referencing the source code, and adding the basic OS package - zephyr.

You can also set environment variables here or some configuration options - such as the target board and similar.

### Before you start

Browse the documentation for the CMake, the CMakeListst.txt part: <https://cmake.org/cmake/help/book/mastering-cmake/chapter/Writing%20CMakeLists%20Files.html>
Browse the list of build system variables given in the Zephyr documentation: <https://docs.zephyrproject.org/latest/develop/application/index.html#important-build-vars>

### TASK2.1: Build system variables in command line

When compiling using the `west build` command, you can specify and build variable using the `-D` command option at the end of the west command line.
To set the board not using the `-b` option but by setting the build variable BOARD, use the command:

```shell
west build -p always config/ -t run -- -DBOARD=qemu_cortex_m0
```

The `-p always` is used to force the build system to update all the files and configuration to get the outputs for the cortex_m0 - the .config generated in previous examples contains the setting for the qemu_x86 and your input would be ignored.

If you see errors at the command line, the build system is confused as you probably redefined the configuration several times. In this case,
return to the default confiburation by calling:

```shell
west config build.dir-fmt default
```

And now it shall work OK.

### TASK2.2: The CMakeLists.txt file

To set a value to a variable, use the cmake *set* command: `set(BOARD qemu_x86)`

1. Set the board in the CMakeLists.txt
2. Try to provide a different board from command line. As you see, the settings from command line have the highest priority and overwrite the configuration given in CMakeLists.txt.
3. Try to set other variables.
4. Try to split the prj.conf file into several files and set up properly in the CMakeLists.txt (This will be usefull: https://docs.zephyrproject.org/latest/develop/application/index.html#application-cmakelists-txt)

Not working? Try to use the task2_2.CMakeLists.txt and the prj files from the soulutions directory.

## TASK3: Environment variables

Several environment variables also affect the build of Zephyr and applications - you can find a list of them here: <https://docs.zephyrproject.org/latest/develop/env_vars.html>.

Work with them like with the other environment variables.
Most of them have the same meaning as the build variables described in the previous section.

A few of them you can use to point to your filesystem if you have more than just one version of the SDK installed or are using a different toolchain.

We mention them in this task mainly to point out that the environment settings affect the compilation and build of applications.

In the task, try setting up a target board using the BOARD variable (in Task2.2 we switched the board to x86, so let's go back to m0 again:):

```shell
export BOARD=qemu_cortex_m0
west build -p always config/ -t run
```

Note: the -p always here is essential, you need to rebuild everything because you have changed the target board.

Check which board the build actually used. Did the BOARD environment variable
win over the `set(BOARD qemu_x86)` still sitting in CMakeLists.txt from TASK2.2,
or did CMakeLists.txt win instead? Either result teaches you something about
where CMake variables come from - try to explain what you observed.

## Putting it all together

You've now set the same kind of option (which board to build for, or what
the boot banner says) through four different mechanisms: editing prj.conf
directly, using menuconfig/guiconfig, a CMakeLists.txt `set()`, and a
command-line `-D` or environment variable. They don't all win equally - each
task asked you to check which one overrides which. Based on what you
observed, try to write down the full priority order, from the setting that
gets overridden most easily to the one that wins no matter what else is
configured.
