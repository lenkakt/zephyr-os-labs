*********************
Zephyr configurations
*********************

In this section, we will focus on all possible parameterizations and configurations that affect the final behaviour of your system. 

These are:
You set the resulting system's source code composition and properties with parameters in prj.conf file using the KConfig syntax.

You set how the system is compiled and assembled from the sub-applications in CMakeLists.txt.

Environment variables also affect the compiler's and other tools' behaviour and can be defined in CMakeLists.txt or as shell variables.

We compose the whole project from different sub-repositories of code using so-called manifests written in YAML. 

This exercise focuses on KConfig (TASK1), CMakeLists.txt (TASK2), and environment variables (TASK3); we'll cover manifests in another section. 



===========================
TASK1: prj.conf and KConfig
===========================

Main goal: To understand how the configuration works, to play with the menu config, to learn how to create the prj.conf

The good point to start is always the documentation, so read the initial description at <https://docs.zephyrproject.org/latest/build/kconfig/index.html>.

In all parts of this task, we will work with these options: 

#. CONFIG_BOOT_DELAY <https://docs.zephyrproject.org/2.7.5/reference/kconfig/CONFIG_BOOT_DELAY.html#std-kconfig-CONFIG_BOOT_DELAY>

#. CONFIG_BOOT_BANNER <https://docs.zephyrproject.org/2.7.5/reference/kconfig/CONFIG_BOOT_BANNER.html#std-kconfig-CONFIG_BOOT_BANNER>

#. CONFIG_BOOT_BANNER_STRING 

----------------
Before you start
----------------

Do not forget to activate the environment to get the West working properly.

.. code-block:: shell

    cd zephyr-os-labs
    source .venv/bin/activate

Read the documentation for the selected configuration options to understand how to set it properly.

-----------------
TASK1.1: prj.conf
-----------------
You can manually write the options into prj.conf.
In the first line, you see the CONFIG_BOOT_DELAY. To get it working, delete the # sign.
Try to set up correctly the boot message - see the options documentation linked in TASK1 description.

To compile and run the emulator, use:

.. code-block:: shell
    
    west build -b qemu_x86 -t run

If you boot successfully, try to play with switching the message on and off.
Now is a perfect time to write any nonsense to prj.conf to find out what will happen during compilation time.

No success? Try the file solutions/task1_1.prj.conf. 

------------------------------
TASK1.2: menuconfig, guiconfig
------------------------------

The menuconfig: :code: `west build -b qemu_x86 -t menuconfig`
The guiconfig: :code: `west build -b qemu_x86 -t menuconfig`


=====================================
TASK2: Compilation and CMakeLists.txt
=====================================



============================
TASK3: Environment variables
============================

