#pragma once

#include "../Boot/multiboot.h"

#include <Memory/Memory.h>

#include <Drivers/ACPI/ACPI.h>
#include <Drivers/ATA/ATA.h>
#include <Drivers/Screen/Screen.h>
#include <Drivers/Interrupt/Interrupt.h>
#include <Drivers/PCI/PCI.h>
#include <Drivers/Serial/Serial.h>
#include <Drivers/PS2/PS2.h>
#include <Filesystem/FAT/Volume.h>

struct Kernel
{
    struct ACPI ACPI;
    struct ATA ATA;
    struct Interrupt Interrupt;
    struct Memory Memory;
    struct PCI PCI;
    struct Screen Screen;
    struct Serial Serial;
    struct PS2 PS2;
};


void Kernel_Constructor(struct Kernel* self, multiboot_info_t* mbi);
void Kernel_InitStorageManager(struct Kernel* self);