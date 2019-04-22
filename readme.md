Copyright Ash Thompson 2019

### Project Goals
Argentum aims to be a simple Operating System Kernel that uses scheduled Tasks and Resources objects 
to manage the computers time and hardware. The Kernel will be exposed through a Lua API that allow 
programmers to write additional drivers and user-space applications.

Argentum is not Unix/Linux/POSIX/Windows compatible and does not aim to be.

### Architecture

#### Devices Objects
Hardware devices in Argentum will be represented by an object. 
Some examples include.

* Communication Devices (Serial, Network)
* Storage Devices (Drive, Volume, Directory, File)
* Input Device (Mouse, Keyboard, Joystick, etc.)
* Output Devices (Console, VGA, Printer, Audio)

#### Tasks
Tasks objects are the executable units that the Scheduler will run depending on their priority, 
dependencies, and other parameters. Two main types of Task will be available.

* Privileged Task objects will be used to implement kernel-level functionality such as device drivers. 
* Userspace Task objects will be used to run code that does not need privileged access to the underlying hardware.

Tasks will be created by the Scheduler, in a ready to execute state. Setting up a Task will involve 
requesting access to Resources and performing program logic, even spawning additional Tasks.

#### Scheduler
Argentum implements a Scheduler class that will be used to manage the lifetime and execution of 
Task objects. The Scheduler will implement task-switching algorithms to determine which Task should 
execute at what time.

### Drivers

* ACPI  
    - Reading FACP
    - Reading ACPI
    - Reading MADT
* ATA 
    - PIO Read/Write Implemented, Needs DMA
* CMOS
* Floppy
* Interrupt
    - Set up IDT
    - Set interrupt masks for each PIC
* IO
    - Reading b/w/l
    - Writing b/w/l
    - Reading b/w/l strings
    - Writing b/w/l strings
* Memory
    - Allocate Memory
    - Free Memory
    - Needs Paging Support
* PCI
    - Reading Config Headers
    - Writing Config Data
    - Brute force scanning. Needs optimisation
* PIT
    - Needs Configuration functions
* PS2
    - Keyboard Working
    - Needs keymap
    - Needs mouse / second port testing
* Screen
    - Working with basic scroll support.
    - Needs scrollback buffer support
* Serial
    - Read COM1
    - Write COM1
    - Needs COM{2,3,4} support.

### Objects
#### Storage Manager
    * FAT
        - FAT16 Volume support
    * MBR
        - Read and parse MBR

##### Filesystem
The root of the file system will consist of the following directories
    - user : Storage for user files
    - libs : Scripts that implement libraries/drivers (non-interactive stuff) 
    - apps : Scripts that implement applications (interactive stuff) 
    - temp : Temporary files

### Lua User Space
All user-space code will be written in Lua. Therefore Argentum's primary goal is to implement a core
kernel capable of running Lua scripts. 
