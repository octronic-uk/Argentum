#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <Objects/Structures/LinkedList.h>
#include <Drivers/ACPI/ACPIConstants.h>
#include <Drivers/ACPI/ACPITableTypes.h>

struct Kernel;

struct ACPIDriver
{
    struct Kernel* Kernel;
    bool Debug;
    uint32_t FacsPointer;
    uint32_t DsdtPointer;
    uint16_t SciInterrupt;
    struct ACPI_RsdpDescriptorV1* V1Header;
    struct LinkedList IoApicRecordPointers;
    struct LinkedList InterruptSourceOverrideRecordPointers;
} ;

bool ACPIDriver_Constructor(struct ACPIDriver* self, struct Kernel* kernel);
void ACPIDriver_Destructor(struct ACPIDriver* self);

void* ACPIDriver_GetEbdaPointer(struct ACPIDriver* self);
void* ACPIDriver_FindRsdpPointer(struct ACPIDriver* self);

uint8_t ACPIDriver_CheckRsdpChecksum(struct ACPIDriver* self, struct ACPI_RsdpDescriptorV1* rsdp, uint8_t version);
uint8_t ACPIDriver_CheckSDTChecksum(struct ACPIDriver* self, struct ACPI_SDTHeader *tableHeader);

void* ACPIDriver_FindSDTBySignature(struct ACPIDriver* self, const char* sig, void* RootSDT, uint8_t version);

void ACPIDriver_SetFacsPointer(struct ACPIDriver* self, uint32_t ptr);
void ACPIDriver_SetSciInterrupt(struct ACPIDriver* self, uint16_t sci);
void ACPIDriver_SetDsdtPointer(struct ACPIDriver* self, uint32_t dsdt);
void APCIDriver_SetDebug(struct ACPIDriver* self, uint8_t debug);

void ACPIDriver_ProcessMADT(struct ACPIDriver* self, struct ACPI_MADT* madt);

void ACPIDriver_DebugIoApic(struct ACPIDriver* self, struct ACPI_MADTRecordIoApic* record);
void ACPIDriver_DebugInterruptSourceOverride(struct ACPIDriver* self, struct ACPI_MADTRecordInputSourceOverride* record);
void ACPIDriver_DebugMADT(struct ACPIDriver* self);