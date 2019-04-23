Copyright Ash Thompson 2019

# Project Goals
Argentum is a bare-bones Operating System Kernel that uses scheduled Resources objects to manage the computers hardware. The Kernel will be exposed through a Lua API that allow programmers to write additional drivers and user-space applications. Argentum is not Unix/Linux/POSIX/Windows compatible and does not aim to be.

# Devices Objects
Hardware devices in Argentum will be represented by an object. Some examples include.

* Communication Devices (Serial, Network)
* Storage Devices (Drive, Volume, Directory, File)
* Input Device (Mouse, Keyboard, Joystick, etc.)
* Output Devices (Console, VGA, Printer, Audio)

## Core

### Drivers
This section outlines the drivers implemented by the kernel and their functions.

#### ACPI 
* Reading FACP
* Reading ACPI
* Reading MADT

#### ATA 
* Select from 1-4 IDE Drives
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

#### IO
* Reading b/w/l
* Writing b/w/l
* Reading b/w/l strings
* Writing b/w/l strings

#### Memory
* Allocate Memory
* Free Memory
* Needs Paging Support

#### PCI
* Reading Config Headers
* Writing Config Data
* Brute force scanning. Needs optimisation

#### PIT
* Needs Configuration functions

#### PS2
* Keyboard Working
* Needs keymap
* Needs mouse / second port testing

#### Screen
* Working with basic scroll support.
* Needs scrollback buffer support

#### Serial
* Read COM1
* Write COM1
* Needs COM{2,3,4} support.

### Objects

#### Storage Manager
#### FAT
* Needs FAT12 support for Floppy
* FAT16 Volume support
#### MBR
* Read and parse MBR

## Filesystem
The root of the file system will consist of the following directories
* /user : Storage for user files
* /libs : Scripts that implement libraries/drivers (non-interactive stuff) 
* /apps : Scripts that implement applications (interactive stuff) 
* /temp : Temporary files
