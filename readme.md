Copyright Ash Thompson 2019

# Project Goals
Argentum is a bare-bones Operating System Kernel that uses Resources objects to manage the computers hardware. The Kernel will be exposed through a Lua API that allow programmers to write additional drivers and user-space applications. Argentum is not Unix/Linux/POSIX/Windows compatible and does not aim to be.

# Code Style
* Argentum is written for readability over speed. The aim is to write self-documenting code, however comments are included where domain-specific knowledge is required. 
* Code is written in a style of 'Object-Driven-C'. Where an object is a struct describing the object's state, and member functions are passed a 'self' variable which is the struct pointer to the object that is being operated on. In a similar style to Python. Global variables should be avioded whenever possible.

# Devices Objects
Hardware devices in Argentum will be represented by an object. Some examples include.

* Communication Devices (Serial, Network)
* Storage Devices (Drive, Volume, Directory, DirectoryEntry)
* Input Device (Mouse, Keyboard, Joystick, etc.)
* Output Devices (Console, VGA, Printer, Audio)

## Core

### Drivers
This section outlines the drivers implemented by the kernel and their primary functions.

#### ACPI 
* Reading FACP
* Reading ACPI
* Reading MADT
* (Still WIP)

#### ATA 
* Probe from 1-4 IDE Drives
* Read Sectors (Currently PIO, Needs DMA)
* Write Sectors (Currently PIO, Needs DMA)

#### CMOS
* Get Floppy Configuration
* Get current time

#### Floppy
* Select Drive
* Read Sectors (DMA)
* Write Sectors (DMA)

#### Interrupt
* Set up IDT
* Set interrupt masks for each PIC
* Register interrupt handler function

#### IO
* Reading b/w/l
* Writing b/w/l
* Reading b/w/l strings
* Writing b/w/l strings

#### Memory
* Allocate Memory
* Free Memory
* Reallocate Memory
* Needs Paging Support

#### PCI
* Reading Config Headers
* Writing Config Data
* Brute force scanning. Needs optimisation

#### PIT
* Needs Configuration functions

#### PS2
* Keyboard input
* Needs mouse / second port testing

#### Screen
* Working with basic scroll support.
* No scrollback buffer support
* Supports x,y indexed writes

#### Serial
* Read COM1
* Write COM1
* Needs COM{2,3,4} support.

### Objects

#### ELF
* Read and parse ELF binary structure (in progress)

#### FAT
* FAT12 support for Floppy (traverse / read / needs write)
* FAT16 support hdd (traverse / read / needs write)

#### Kernel
* High level Kernel Object

#### LinkedList
* Singly linked list (dynamically allocated)

#### MBR
* Read and parse MBR

#### RamDisk
* Supports the creation of arbitrary sized ram-disks

#### Storage Manager
* Enumerate ATA / Floppy / RamDisk devices
* File lookup via path: ata0p1://dir/subdir/file.ext, ram0://file.ext, etc.

## Typical Filesystem
The root file system aims to be a simple as possible. It will consist of the following directories
* /user : Storage for user files
* /libs : Scripts that implement libraries/drivers (non-interactive stuff) 
* /apps : Scripts that implement applications (interactive stuff) 
* /temp : Temporary files
