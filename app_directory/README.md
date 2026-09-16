# The application directory template

This is a void application, used to create the other apps. It cotnains only the necessary files: the sample.yaml, CMakeLists.txt, src/main.c and this README.
To set up a file structure for another app:
1. Copy this files into target directory.
2. Update CMakeLists - you need to update the project name.
3. Update sample.yaml as you need.
4. Update this Readme to describe the app (update before you start in next paragraph - add link to app directory to last command).

## Before you start

Make sure you're in the `zephyr-os-labs` directory with the virtual environment activated.

Build the example, clean all the old stuff and ensure that it is working:

```shell
west build -p always -b qemu_cortex_m0 -t run ./my-app
```
