#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <Objects/LinkedList/LinkedList.h>
#include <Drivers/ACPI/ACPIConstants.h>
#include <Drivers/ACPI/ACPITableTypes.h>

struct ACPIDriver
{
    bool Debug;
    uint32_t FacsPointer;
    uint32_t DsdtPointer;
    uint16_t SciInterrupt;
    ACPI_RsdpDescriptorV1* V1Header;
    LinkedList IoApicRecordPointers;
    LinkedList InterruptSourceOverrideRecordPointers;
} ;
typedef struct ACPIDriver ACPIDriver;

bool ACPIDriver_Constructor(ACPIDriver* self);
void ACPIDriver_Destructor(ACPIDriver* self);

void* ACPIDriver_GetEbdaPointer(ACPIDriver* self);
void* ACPIDriver_FindRsdpPointer(ACPIDriver* self);

uint8_t ACPIDriver_CheckRsdpChecksum(ACPIDriver* self, ACPI_RsdpDescriptorV1* rsdp, uint8_t version);
uint8_t ACPIDriver_CheckSDTChecksum(ACPIDriver* self, ACPI_SDTHeader *tableHeader);

void* ACPIDriver_FindSDTBySignature(ACPIDriver* self, const char* sig, void* RootSDT, uint8_t version);

void ACPIDriver_SetFacsPointer(ACPIDriver* self, uint32_t ptr);
void ACPIDriver_SetSciInterrupt(ACPIDriver* self, uint16_t sci);
void ACPIDriver_SetDsdtPointer( ACPIDriver* self, uint32_t dsdt);
void APCIDriver_SetDebug(ACPIDriver* self, uint8_t debug);

void ACPIDriver_ProcessMADT(ACPIDriver* self, ACPI_MADT* madt);

void ACPIDriver_DebugIoApic(ACPIDriver* self, ACPI_MADTRecordIoApic* record);
void ACPIDriver_DebugInterruptSourceOverride(ACPIDriver* self, ACPI_MADTRecordInputSourceOverride* record);
void ACPIDriver_DebugMADT(ACPIDriver* self);