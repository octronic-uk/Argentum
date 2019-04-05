#pragma once

#include "../Boot/multiboot.h"


#include <Drivers/ACPI/ACPIDriver.h>
#include <Drivers/ATA/ATADriver.h>
#include <Drivers/Interrupt/InterruptDriver.h>
#include <Drivers/Memory/MemoryDriver.h>
#include <Drivers/PCI/PCIDriver.h>
#include <Drivers/PIT/PITDriver.h>
#include <Drivers/PS2/PS2Driver.h>
#include <Drivers/Screen/ScreenDriver.h>
#include <Drivers/Serial/SerialDriver.h>

#include <Objects/StorageManager/StorageManager.h>

struct Kernel
{
    multiboot_info_t* MultibootInfo;
    // Drivers
    struct ACPIDriver ACPI;
    struct ATADriver ATA;
    struct InterruptDriver Interrupt;
    struct MemoryDriver Memory;
    struct PCIDriver PCI;
    struct PITDriver PIT;
    struct PS2Driver PS2;
    struct ScreenDriver Screen;
    struct SerialDriver Serial;
    // Objects
    struct StorageManager StorageManager;
};


bool Kernel_Constructor(struct Kernel* self, multiboot_info_t* mbi);
bool Kernel_InitDrivers(struct Kernel* self);
bool Kernel_InitObjects(struct Kernel* self);