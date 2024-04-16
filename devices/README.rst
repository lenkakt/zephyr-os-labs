*********************
The device management
*********************

In this lab, you shall learn how to manage devices in Zephyr OS.
You will learn how to create a device object, add resources to it, and manage the device object.
We will spent some time with device trees.

================
Before you start
================

1. Unless otherwise specified, it is assumed that you enter commands in the zephyr-os-labs directory.
2. Do not forget to activate the environment using command: 

.. code-block:: shell

    cd zephyr-os-labs
    source .venv/bin/activate

3. Build the example, clean all the old stuff and ensure that it is working:

.. code-block:: shell

    west build -p always -b qemu_cortex_m0 -t run ./devices


