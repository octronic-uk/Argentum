#pragma once

#include <stdbool.h>
#include <Boot/multiboot.h>

// Drivers
#include <Drivers/ACPI/ACPIDriver.h>
#include <Drivers/ATA/ATADriver.h>
#include <Drivers/Floppy/FloppyDriver.h>
#include <Drivers/Interrupt/InterruptDriver.h>
#include <Drivers/Memory/MemoryDriver.h>
#include <Drivers/PCI/PCIDriver.h>
#include <Drivers/PIT/PITDriver.h>
#include <Drivers/PS2/PS2Driver.h>
#include <Drivers/Screen/TextMode/TextModeDriver.h>
#include <Drivers/Serial/SerialDriver.h>
#include <Drivers/Screen/Vga/VgaDriver.h>
#include <Objects/HardwareManager/HardwareManager.h>

struct Kernel
{
    // Variables
    multiboot_info_t* MultibootInfo;
    volatile bool RunLoop;
    // Drivers
    ACPIDriver ACPI;
    ATADriver ATA;
    FloppyDriver Floppy;
    InterruptDriver Interrupt;
    MemoryDriver Memory;
    PCIDriver PCI;
    PITDriver PIT;
    PS2Driver PS2;
    TextModeDriver TextMode;
    VgaDriver Vga;
    SerialDriver Serial;
    // Objects
    HardwareManager HardwareManager;
};

typedef struct Kernel Kernel;

bool Kernel_Constructor(Kernel* self, multiboot_info_t* mbi);
bool Kernel_InitDrivers(Kernel* self);
bool Kernel_InitObjects(Kernel* self);
void Kernel_Destructor(Kernel* self);
void Kernel_DestroyObjects(Kernel* self);
void Kernel_DestroyDrivers(Kernel* self);

void Kernel_TestDrivers(Kernel* self);
void Kernel_TestObjects(Kernel* self);

void Kernel_Run(Kernel* self);