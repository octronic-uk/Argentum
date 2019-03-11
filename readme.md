Copyright Ashley Thompson 2019

# Taskie

## Abstract
This document introduces the reader to the goals and architectural design of the 
Taskie Operating System.

## Project Goals
Taskie aims to be a simple object-driven Operating System Kernel that uses scheduled 
Tasks and Resources objects to manage the computers time and hardware. The Kernel 
will be exposed through an object-oriented API that allow programmers to write 
user-space applications.

## Architecture

### Scheduler
Taskie implements a Scheduler class that will be used to manage the lifetime and 
execution of Task objects. The Scheduler will implement task-switching algorithms 
to determine which Task should execute at what time.

### Tasks
Tasks objects are the executable units that the Scheduler will run depending on 
their priority, dependencies, and other parameters. Two main types of Task will 
be available.

* PrivilegedTask objects will be used to implement kernel-level functionality 
  such as device drivers. 
* UserSpaceTask objects will be used to run code that does not need privileged 
  access to the underlying hardware.

Tasks will be created by the Scheduler, in a ready to execute state. Setting up 
a Task will involve requesting access to Resources and performing program logic, 
even spawning additional Tasks.

## Managing Hardware Devices

All hardware devices (aside from the CPU itself) in Taskie will be represented 
by the Resource class. Specialisations will be used when necessary to add specific 
functionality to a Resource. Some Resource Examples include.

* A Memory Region (MMIO, DMA, RAM, SWAP)
* Network (Establish Connection, listen port, send port)
* Storage Devices (SSD/NVME/FLASH/etc)
* Graphical Devices (VGA, GPU Accelerator, Printer, etc)
* Expansion Busses (SATA, PCIe, USB, I2C, Thunderbolt)
* Audio Devices (Capture & Recording)
* Input Device (Mouse, Keyboard, Joystick, etc.)

## Benefits of this Design
* Asynchronous Execution by design

##Notes
* Scheduler type (Initially RoundRobin)
* Task Resume/Suspend capability
* Task execution rollback capability
* Memory always zero when freed

