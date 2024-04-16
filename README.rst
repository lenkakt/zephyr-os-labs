***************
Zephyr OS Labs
***************

This repository contains Zephyr-based lab exercises for the standard university Operating Systems course. The main goal is to use Zephyr as an example OS to practice the basics of operating systems theory.
Each lab takes approx. 90 minutes. The lecture should start with a short introduction to the topic given by the lecturer. Only the labs are presented here; the intros are now in TBD mode.

The practical labs cover the following topics:

#. `Hello <hello/>`_ - Installing and setting up all the environment
#. `Config <config/>`_ - KConfig and all the other configurations and parameters in Zephyr
#. `Kernel <kernel_lab/>`_ - Kernel configuration, working with kernel API
#. `Threads <threads/>`_ - Working with threads
#. `Scheduling <scheduling/>`_ - Working with scheduler - types of job scheduling, performance measurement
#. `Synchro <synchro/>`_ - Job synchronization - locks, semaphores, atomic operations
#. `Memory <memory/>`_ - Memory management, assignment and protection
#. `Devices <devices/>`_ - Device management - device trees, modifying them, device controller
#. `Filesystems <filesystems/>`_ - Working with file systems. 


-------------
Target boards
-------------
The first release is tested on qemu_x86 and qemu_cortex_m0 and qemu_cortex_rt (user-space). Later on, the examples for the Hardwario Chester platform, which is used in our labs, shall be added.

---------------
Target audience
---------------
Our primary target audience are students of Computer Science in the 2nd year of university study; we assume basic knowledge of the Linux OS environment and command line, as well as basic knowledge of C and C++. 

----------------------
Host operating systems
----------------------
All the labs were tested on Linux (Ubuntu 22.04 LTS) and in WSL (Windows Subsystem for Linux) on Windows 10. The labs should work on other Linux distributions as well.

------------------
How to do the labs
------------------
Each lab has its own directory. The lab directory contains a README file with the lab description and a solution directory containing the solution to the given lab. The lab directory also contains a Makefile that can be used to build the lab.

------------------------
How to use the solutions
------------------------
Each lab contains a "solutions" directory with the solution to the given lab. 
The solution can be a proposed main.c or a configuration file or both.

The filenames are using following patterns:

- Kconfig_X.Y - the configuration file for the given task. It shall be renamed to Kconfig and moved to the lab directory.

- task_X.Y.c - the main file for the given task. It shall be renamed to main.c and moved to the lab directory.

- prj_X.Y.conf - the project configuration file for the given task. It shall be renamed to prj.conf and moved to the lab directory.

**Maintainer**: Lenka Kosková Třísková, lenka.koskova.triskova@tul.cz
The course outline is based on my teaching at Technical University of Liberec. 
