***************
Zephyr OS Labs
***************

This repository contains Zephyr-based lab exercises for the standard university Operating Systems course. The main goal is to use Zephyr as an example OS to practice the basics of operating systems theory.
Each lab takes approx. 90 minutes. The lecture should start with a short introduction to the topic given by the lecturer. Only the labs are presented here; the intros are now in TBD mode.

The practical labs cover the following topics:

#. `Hello <hello/>`_ - Installing and setting up all the environment
#. `Config <config/>`_ - KConfig and all the other configurations and parameters in Zephyr
#. `Kernel <kernel_lab/>`_ - Kernel configuration, working with kernel API
#. `Threads* <threads>`_- Creating and working with threads
#. *SysCalls* - System calls
#. `Threads <threads/>`_ - Creating and working with threads
#. *Interrupts* - Working with interrupts
#. *Scheduling* - Working with scheduler - types of job scheduling, performance measurement
#. *Synchro* - Job synchronization - locks, semaphores, atomic operations
#. *Memory* - Memory management, assignment and protection
#. *DeviceTree* - Device management - device trees, modifying them, device controller
#. *Filesystems* - Working with file systems 
#. *Security*

-------------
Target boards
-------------
The first release is created for the qemu_x86 and qemu_cortex_m0. Later on, the examples for the Hardwario Chester platform, which is used in our labs, shall be added.

---------------
Target audience
---------------
Our primary target audience is students of Computer Science in the 2nd year of university study; we assume basic knowledge of the Linux OS environment and command line, as well as basic knowledge of C and C++. 
We have Linux computers in the labs, so we do not describe command variations and settings in Windows.

**Maintainer**: Lenka Kosková Třísková, lenka.koskova.triskova@tul.cz
The course outline is based on my teaching at Technical University of Liberec. 
