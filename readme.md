Copyright Ash Thompson 2019

### Project Goals
Argentum aims to be a simple object-driven Operating System Kernel that uses scheduled Tasks and Resources objects to manage the computers time and hardware. The Kernel will be exposed through an object-oriented API that allow programmers to write user-space applications.

### Architecture

#### Devices Objects
Hardware devices (aside from the CPU itself) in Argentum will be represented by an object. Some examples include.

* Communication Devices (Serial, Network)
* Storage Devices (Drive, Volume, Directory, File)
* Input Device (Mouse, Keyboard, Joystick, etc.)
* Output Devices (VGA, GPU, Printer, Audio)

#### Tasks
Tasks objects are the executable units that the Scheduler will run depending on their priority, dependencies, and other parameters. Two main types of Task will be available.

* Privileged Task objects will be used to implement kernel-level functionality such as device drivers. 
* Userspace Task objects will be used to run code that does not need privileged access to the underlying hardware.

Tasks will be created by the Scheduler, in a ready to execute state. Setting up a Task will involve requesting access to Resources and performing program logic, even spawning additional Tasks.

#### Scheduler
Argentum implements a Scheduler class that will be used to manage the lifetime and execution of Task objects. The Scheduler will implement task-switching algorithms to determine which Task should execute at what time.

### API

#### MemoryManager
    MemMan_KAllocate
    MemMan_KFree
    MemMan_UAllocate
    MemMan_UFree

#### StorageManager
    StorMan_ListDrives
    StorMan_GetDrive

#### Drive
    Drive_ListVolumes
    Drive_GetVolume

#### Volume
    Volume_CreateDirectory
    Volume_RemoveDirector
    Volume_GetDirectory

#### Directory
    Directory_List
    Directory_Rename

#### File
    File_Create
    File_Delete
    File_Rename
    File_Read
    File_Write

## Current Status
### Drivers

* ACPI  
    - Reading FACP
    - Reading ACPI
    - Reading MADT
* ATA 
    - PIO Read/Write Implemented, Needs DMA
* Interrupt
    - Reading b/w/l
    - Writing b/w/l
    - Reading b/w/l strings
    - Writing b/w/l strings
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

### Filesystem
* API
    - Drafting high level api for file access
* FAT
    - FAT16 Volume support
* MBR
    - Read MBR into memory

