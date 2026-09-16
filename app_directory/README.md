# The application directory template

This is a void application, used to create the other apps. It cotnains only the necessary files: the sample.yaml, CMakeLists.txt, src/main.c and this README.
To set up a file structure for another app:
1. Copy this files into target directory.
2. Update CMakeLists - you need to update the project name.
3. Update sample.yaml as you need.
4. Update this Readme to describe the app (update before you start in next paragraph - add link to app directory to last command).

## Before you start

1. Unless otherwise specified, it is assumed that you enter commands in the zephyr-os-labs directory.
2. Do not forget to activate the environment using command:

```shell
cd zephyr-os-labs
source .venv/bin/activate
```

3. Build the example, clean all the old stuff and ensure that it is working:

```shell
west build -p always -b qemu_cortex_m0 -t run ./my-app
```
