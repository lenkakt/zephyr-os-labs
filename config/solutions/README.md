# Config lab - solutions and answers

## Files

| File | Task | How to use |
|---|---|---|
| `task1_1.prj.conf` | TASK1.1 | rename to `prj.conf` |
| `task1_2.main.c` | TASK1.2 | rename to `main.c` and move to `src/` |
| `task2_2.CMakeLists.txt` | TASK2.2 | rename to `CMakeLists.txt`, copy `task2_1_A.conf` and `task2_1_B.conf` next to it |

## TASK1.2

* `CONFIG_BOOT_BANNER=y` becomes `#define CONFIG_BOOT_BANNER 1` in `autoconf.h`.
* When the banner is switched off (`=n`), `CONFIG_BOOT_BANNER` is **not defined at all** -
  and neither is `CONFIG_BOOT_BANNER_STRING`, because it depends on `BOOT_BANNER`.
* An int option such as `CONFIG_BOOT_DELAY` is always defined, also with the value 0.
* `if (IS_ENABLED(CONFIG_BOOT_BANNER)) { ... CONFIG_BOOT_BANNER_STRING ... }` fails to compile
  with the banner off: `IS_ENABLED()` is a normal C condition, the code in the branch must be
  valid C, and `CONFIG_BOOT_BANNER_STRING` is an undefined identifier. With `#if` the code is
  removed by the preprocessor before the compiler sees it.

## TASK1.3 - menuconfig vs. prj.conf

Tested with Zephyr 4.4:

1. menuconfig saves only to `build/zephyr/.config`; `prj.conf` stays unchanged.
2. After a rebuild, the **menuconfig value wins** - the `.config` in the build directory is used.
3. When the **content** of `prj.conf` changes (just `touch` is not enough), the build system
   merges the configuration again: board defconfig, then `prj.conf`. The menuconfig change
   is **lost** and the `prj.conf` value wins.
4. `-p always` deletes the build directory - the menuconfig change is lost too.

Conclusion: menuconfig is a tool to explore and try the options. A setting that should
survive belongs to `prj.conf` (or another `.conf` fragment).

Priority of the Kconfig sources (weakest first): Kconfig default → board defconfig →
`prj.conf` and other fragments → menuconfig (only until the next merge).

## TASK2.1 - who chooses the board

Tested with Zephyr 4.4 (weakest first):

| Source | Wins over |
|---|---|
| `set(BOARD ...)` in `CMakeLists.txt` | – |
| `west config build.board` | `set()` |
| environment variable `BOARD` | `build.board`, `set()` |
| `-b` or `-- -DBOARD=...` on the command line | everything |

Watch out: if `west config -l` shows a `build.board` line (e.g. left there 
from building some other application), the `set(BOARD)` in `CMakeLists.txt` is silently ignored.
Remove it with `west config -d build.board`.

## TASK2.2

The two fragments are listed in `CONF_FILE` (it replaces `prj.conf`). Alternatively, keep
`prj.conf` and add the other files with `EXTRA_CONF_FILE`. The build output shows one
`Merged configuration` line per fragment.
