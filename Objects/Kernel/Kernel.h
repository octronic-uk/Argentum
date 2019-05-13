#pragma once

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

// Objects
#include <Objects/StorageManager/StorageManager.h>
#include <Objects/GraphicsManager/GraphicsManager.h>

struct Kernel
{
    multiboot_info_t* MultibootInfo;
    // Drivers
    ACPIDriver ACPI;
    struct ATADriver ATA;
    struct FloppyDriver Floppy;
    struct InterruptDriver Interrupt;
    struct MemoryDriver Memory;
    struct PCIDriver PCI;
    struct PITDriver PIT;
    struct PS2Driver PS2;
    struct TextModeDriver TextMode;
    struct VgaDriver Vga;
    struct SerialDriver Serial;
    // Objects
    struct StorageManager StorageManager;
    struct GraphicsManager GraphicsManager;
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