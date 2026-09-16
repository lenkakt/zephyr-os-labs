# The filesystems in Zephyr

This lab demonstrates filesystems in Zephyr.

## Before you start

1. Unless otherwise specified, it is assumed that you enter commands in the zephyr-os-labs directory.
2. Do not forget to activate the environment using command:

```shell
cd zephyr-os-labs
source .venv/bin/activate
```

3. Build the example, clean all the old stuff and ensure that it is working:

```shell
west build -p always -b qemu_cortex_m0 -t run ./filesystems
```
