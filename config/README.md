# Zephyr configurations

In this lab, you will find out **where the inputs of a Zephyr build are** and **how they change
the code that is finally compiled**.

The same source code can produce very different systems. What ends up in `zephyr.elf`
depends on many inputs:

| Input | Where it is | What it decides |
|---|---|---|
| Board | `-b qemu_x86`, `BOARD` variable, west configuration | which CPU, memory and peripherals; brings its own defconfig and devicetree |
| Board defconfig | `zephyr/boards/<vendor>/<board>/<board>_defconfig` | default Kconfig options for the board |
| Board devicetree | `zephyr/boards/<vendor>/<board>/<board>.dts` | hardware description (see the Devices lab) |
| Application configuration | `prj.conf` and other `.conf` fragments | Kconfig options of your application |
| Kconfig files | `Kconfig` in Zephyr sources (and optionally in your app) | which options exist, their types, defaults and dependencies |
| Build description | `CMakeLists.txt` | which sources are compiled, build variables |
| Command line and environment | `west build -b ... -- -DVAR=...`, `export VAR=...` | build variables, e.g. the board |
| West configuration | `west config -l` | e.g. a default board (`build.board`) |
| West manifest | `west.yml` | which version of Zephyr and modules is used |

And these are the outputs generated in the build directory (`build/`):

| Output | What it is |
|---|---|
| `build/zephyr/.config` | the final, merged Kconfig configuration |
| `build/zephyr/include/generated/zephyr/autoconf.h` | the same configuration as C macros (`#define CONFIG_...`) |
| `build/zephyr/zephyr.dts` | the final, merged devicetree |
| `build/zephyr/zephyr.elf`, `zephyr.bin` | the resulting system |

## Before you start

Make sure you're in the `zephyr-os-labs` directory with the virtual environment activated.

Build the example as it is, from scratch (`-p always`, "pristine" - throws away any previous
build output and reconfigures everything; you'll want this every time you change boards or
Kconfig fragments in this lab, since otherwise west can keep using stale, cached
configuration), to be sure it is working:

```shell
west build -p always -b qemu_x86 config/ -t run
```

Stop the emulator with Ctrl+A, X.

## TASK0: Where are the inputs?

The build system tells you which inputs it used - you just have to read its output.

1. Scroll back to the beginning of the build output and find these lines:

   ```text
   -- Board: qemu_x86
   -- Found BOARD.dts: .../boards/qemu/x86/qemu_x86.dts
   Loaded configuration '.../boards/qemu/x86/qemu_x86_defconfig'
   Merged configuration '.../config/prj.conf'
   ```

2. Open the board defconfig file (the path is in the `Loaded configuration` line).
   Which options does the board itself set? Which of them configure the kernel
   (hint: look for the scheduler)?
3. Build the application for `qemu_cortex_m0` and compare: which files are loaded now?

   ```shell
   west build -p always -b qemu_cortex_m0 config/ -t run
   ```

4. The board is an input bundle: by choosing it, you choose a defconfig and a devicetree too.

Remember: whenever a build behaves unexpectedly, read the `Board`, `Loaded configuration`
and `Merged configuration` lines first.

## TASK1: From prj.conf to the code

The configuration options are defined in Kconfig files and set in `prj.conf`.
They are not used directly in the code - the build system first merges all inputs into
`build/zephyr/.config` and then generates a header file `autoconf.h` with C macros.
This header is automatically included in every compiled file - the kernel, drivers and your
application.

Start with the documentation: <https://docs.zephyrproject.org/latest/build/kconfig/index.html>.

We will play with these options:

1. CONFIG_BOOT_DELAY <https://docs.zephyrproject.org/latest/kconfig.html#CONFIG_BOOT_DELAY>
2. CONFIG_BOOT_BANNER <https://docs.zephyrproject.org/latest/kconfig.html#CONFIG_BOOT_BANNER>
3. CONFIG_BOOT_BANNER_STRING <https://docs.zephyrproject.org/latest/kconfig.html#CONFIG_BOOT_BANNER_STRING>

### TASK1.1: prj.conf

In the first line of `prj.conf`, you see the CONFIG_BOOT_DELAY. To get it working, delete the `#` sign.

CONFIG_BOOT_BANNER and CONFIG_BOOT_BANNER_STRING aren't in `prj.conf` yet - add both lines
yourself to set up the boot message (see the options documentation linked above for the
exact syntax).

Compile and run:

```shell
west build -b qemu_x86 config/ -t run
```

If you boot successfully, try switching the message on and off.
Now is a perfect time to write any nonsense to `prj.conf` to find out what happens during
the build.

No success? Try the file `solutions/task1_1.prj.conf`.

### TASK1.2: Follow the option into the code

1. Open `build/zephyr/.config` and find your three options. Are they the same as in `prj.conf`?
2. Open `build/zephyr/include/generated/zephyr/autoconf.h` and find them again.
   How does `CONFIG_BOOT_BANNER=y` look like as a C macro? And what happens with
   `CONFIG_BOOT_BANNER_STRING` when you switch the banner off?
3. Find where the kernel uses the options. They are in the file `kernel/banner.c`
   in the Zephyr sources:

   ```shell
   grep -n "CONFIG_BOOT" $ZEPHYR_BASE/kernel/banner.c
   ```

   You will see lines like `#if defined(CONFIG_BOOT_DELAY) && (CONFIG_BOOT_DELAY > 0)`.
   The option decides **which code is compiled at all** - when the delay is 0, the delay code
   is not in `zephyr.elf`.
4. Use the options in your own `src/main.c`: print whether the boot banner is on, its
   text, and the boot delay. You need two techniques:
   * `#if defined(CONFIG_...)` / `#ifdef` - the code inside is compiled only when the option is set;
   * `IS_ENABLED(CONFIG_...)` - a normal C condition, the compiler removes the dead branch.

   Try `if (IS_ENABLED(CONFIG_BOOT_BANNER)) { printf("%s\n", CONFIG_BOOT_BANNER_STRING); }`
   and switch the banner off. Why does the build fail, although the branch would never run?
   (Hint: look into `autoconf.h` again.)

No success? Try the file `solutions/task1_2.main.c`.

### TASK1.3: menuconfig and guiconfig

There are many configuration options, and they depend on each other. Writing a configuration
from scratch in a file and considering all the dependencies is challenging; no one asks you
to do it!

The right way is to use ready-made tools that group the options into menus: the GUI tool
(guiconfig) or the text menus in the terminal (menuconfig). The menu structure is defined in
the Kconfig files in the Zephyr sources; the options used in this lab are defined in
`$ZEPHYR_BASE/kernel/Kconfig`.

1. Keep your settings in `prj.conf`.
2. Start one of the interfaces, change the banner text and save it.

   ```shell
   west build -b qemu_x86 config/ -t menuconfig
   west build -b qemu_x86 config/ -t guiconfig
   ```

3. Rebuild and run. Which text do you see - the one from `prj.conf` or the one from the menu?
4. Where did menuconfig save your change? Look at `build/zephyr/.config` and at `prj.conf`.
5. Now change the banner text in `prj.conf`, rebuild and run. Which text wins now?
6. Set a new text in menuconfig again and then rebuild with `-p always`. What happened to
   your menuconfig change?

Think about it: is menuconfig a good place to keep the configuration of your project?
Where should a setting go, if you want it to survive?

## TASK2: The board and the build variables

The system build is orchestrated by CMake. The general description is given in the Zephyr
documentation: <https://docs.zephyrproject.org/latest/build/cmake/index.html>.

The entry point for CMake is the application's `CMakeLists.txt` file. The initial one
contains only the basic template: the application's name, the source code and the Zephyr
package.

Before you start, browse:

* the CMakeLists.txt part of the CMake documentation:
  <https://cmake.org/cmake/help/book/mastering-cmake/chapter/Writing%20CMakeLists%20Files.html>
* the list of important build variables:
  <https://docs.zephyrproject.org/latest/develop/application/index.html#important-build-vars>
* the list of environment variables: <https://docs.zephyrproject.org/latest/develop/env_vars.html>

### TASK2.1: Who chooses the board?

The board can be chosen in five different ways. Try them one by one and always check the
`-- Board:` line in the build output.

1. **West configuration.** List the west configuration:

   ```shell
   west config -l
   ```

   Is there a `build.board` line? If yes, west uses this board whenever you don't give one.

2. **CMakeLists.txt.** Add `set(BOARD qemu_x86)` **before** `find_package(Zephyr)` and build
   without any board:

   ```shell
   west build -p always config/ -t run
   ```

3. **Environment variable.**

   ```shell
   export BOARD=qemu_cortex_m0
   west build -p always config/ -t run
   ```

4. **Build variable on the command line** (everything after `--` goes to CMake):

   ```shell
   west build -p always config/ -t run -- -DBOARD=qemu_cortex_m0
   ```

5. **The `-b` option** - the one you have used so far.

Combine them (e.g. `set()` in CMakeLists.txt together with the environment variable) and find
out which one wins. Do not forget to `unset BOARD` when you are done.

### TASK2.2: Configuration fragments

The configuration does not have to be in one `prj.conf` file. Split it into two files
(e.g. one with the boot delay and one with the banner) and tell the build system to use both.
This will be useful:
<https://docs.zephyrproject.org/latest/develop/application/index.html#application-cmakelists-txt>

Then build and find the `Merged configuration` lines in the build output - are both files
there?

Not working? Try `solutions/task2_2.CMakeLists.txt` and the `task2_1_*.conf` files from the
solutions directory.

## Putting it all together

You've now set the same kind of option (which board to build for, or what the boot banner
says) through several different mechanisms. They don't all win equally. Based on what you
observed, write down:

1. the priority order of the ways to choose the board, from the one that gets overridden
   most easily to the one that wins no matter what else is configured;
2. the priority order of the board defconfig, `prj.conf` and menuconfig - and when the
   menuconfig changes get lost.
