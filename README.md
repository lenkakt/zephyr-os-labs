# Zephyr OS Labs

This repository contains Zephyr-based lab exercises for the standard university Operating Systems course. The main goal is to use Zephyr as an example OS to practice the basics of operating systems theory.
Each lab takes approx. 90 minutes. The lecture should start with a short introduction to the topic given by the lecturer. Only the labs are presented here; the intros are now in TBD mode.

The practical labs cover the following topics:

1. [Hello](hello/) - Installing and setting up all the environment
2. [Config](config/) - KConfig and all the other configurations and parameters in Zephyr
3. [Kernel](kernel_lab/) - Kernel configuration, working with kernel API
4. [Threads](threads/) - Working with threads
5. [Scheduling](scheduling/) - Working with scheduler - types of job scheduling, performance measurement
6. [Synchro](synchro/) - Job synchronization - locks, semaphores, atomic operations
7. [Memory](memory/) - Memory management, assignment and protection
8. [Devices](devices/) - Device management - device trees, modifying them, device controller
9. [Filesystems](filesystems/) - Working with file systems.

## Target boards

The first release is tested on qemu_x86 and qemu_cortex_m0 and qemu_cortex_rt (user-space).

## Running in the cloud

[![Open in GitHub Codespaces](https://github.com/codespaces/badge.svg)](https://codespaces.new/lenkakt/zephyr-os-labs)

Prefer not to install anything locally? Click the badge above to open this
repository in [GitHub Codespaces](https://github.com/features/codespaces) —
a browser-based VS Code with Zephyr, west and the toolchains for the QEMU
boards above already installed. Building it for the first time may take a
few minutes.

This is a supplement to the classroom setup below, not a replacement for
it — the university's lab computers keep working exactly as described.
Codespaces builds and runs the labs in the same generic QEMU boards used
above; it does not emulate any real target hardware, since GitHub
Codespaces runs in the cloud and has no USB access to flash a physical
board. If you're working with real hardware (e.g. a Seeeduino XIAO), that
part of the workflow stays local, over USB.

## Target audience

Our primary target audience are students of Computer Science in the 2nd year of university study; we assume basic knowledge of the Linux OS environment and command line, as well as basic knowledge of C and C++.

## Host operating systems

All the labs were tested on Linux (Ubuntu 22.04 LTS) and in WSL (Windows Subsystem for Linux) on Windows 10. The labs should work on other Linux distributions as well.

## How to do the labs

Each lab has its own directory. The lab directory contains a README file with the lab description and a solution directory containing the solution to the given lab. The lab directory also contains a Makefile that can be used to build the lab.

## How to use the solutions

Each lab contains a "solutions" directory with the solution to the given lab.
The solution can be a proposed main.c or a configuration file or both.

The filenames are using following patterns:

- Kconfig_X.Y - the configuration file for the given task. It shall be renamed to Kconfig and moved to the lab directory.

- task_X.Y.c - the main file for the given task. It shall be renamed to main.c and moved to the lab directory.

- prj_X.Y.conf - the project configuration file for the given task. It shall be renamed to prj.conf and moved to the lab directory.

**Maintainer**: Lenka Kosková Třísková, lenka.koskova.triskova@tul.cz
The course outline is based on my teaching at Technical University of Liberec.
