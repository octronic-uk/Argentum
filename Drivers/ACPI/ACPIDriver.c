#include "ACPIDriver.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <Objects/Kernel/Kernel.h>
#include <Drivers/IO/IODriver.h>
#include <Drivers/Memory/MemoryDriver.h>
#include <Drivers/PS2/PS2Driver.h>
#include <Objects/Structures/LinkedList.h>

extern struct Kernel _Kernel;

bool ACPIDriver_Constructor(struct ACPIDriver* self)
{
    printf("ACPI: Constructing\n");
    self->Debug = false;
    self->FacsPointer = 0;
    self->DsdtPointer = 0;
    self->SciInterrupt = 0;
    self->V1Header = 0;

    LinkedList_Constructor(&self->InterruptSourceOverrideRecordPointers, &_Kernel.Memory);
    LinkedList_Constructor(&self->IoApicRecordPointers, &_Kernel.Memory);

    void* rsdpPointer = ACPIDriver_FindRsdpPointer(self);

    if (self->Debug)
    {
        printf("ACPI: RSDP Pointer found: 0x%x\n",rsdpPointer);
        PS2Driver_WaitForKeyPress("ACPI Pause");
    }


    self->V1Header = (struct ACPI_RsdpDescriptorV1*)rsdpPointer;

    if (self->Debug) 
    {
        printf("ACPI: OEM ID (%s)\n",self->V1Header->OEMID);
    }

    // ACPI V1
    uint8_t version  = self->V1Header->Revision;
    if (version == 0)
    {
        if (self->Debug) 
        {
            printf("ACPI: Version == 1.0\n");
        }
    }
    // ACPI v2 to v6.1
    else if (version == 2)
    {
        if (self->Debug) 
        {
            printf("ACPI: Version >= 2.0\n");
        }
    }
    else
    {
        if (self->Debug)
        {
            printf("ACPI: Version is INVALID\n");
            PS2Driver_WaitForKeyPress("ACPI Pause");
        }
        return false;
    }

    if (!ACPIDriver_CheckRsdpChecksum(self, self->V1Header,version))
    {
        if (self->Debug)
        {
            printf("ACPI: RSDT Checksum is invalid\n");
            PS2Driver_WaitForKeyPress("ACPI Pause");
        }
        return false;
    }

    void* rsdt = self->V1Header->RsdtAddress;
    if (self->Debug) 
    {
        printf("ACPI: Using RsdtAddress 0x%x\n",rsdt);
        PS2Driver_WaitForKeyPress("ACPI Pause");
    }

    struct ACPI_FADT* facp = ACPIDriver_FindSDTBySignature(self, ACPI_SIG_FACP, rsdt,version);
    if (facp)
    {
       if (self->Debug) 
       {
            printf("ACPI: Found FACP\n");
            PS2Driver_WaitForKeyPress("ACPI Pause");
       }
       ACPIDriver_SetFacsPointer(self, facp->FirmwareCtrl);
       ACPIDriver_SetDsdtPointer(self, facp->Dsdt);
       ACPIDriver_SetSciInterrupt(self, facp->SCI_Interrupt);
    }

    struct ACPI_SDTHeader* madt_sdt = ACPIDriver_FindSDTBySignature(self, ACPI_SIG_APIC, rsdt,version);

    if (madt_sdt)
    {
        struct ACPI_MADT* madt = (struct ACPI_MADT*)madt_sdt;
        ACPIDriver_ProcessMADT(self, madt);
    }
    return true;
}

void ACPIDriver_Destructor(struct ACPIDriver* self)
{
    if (self->Debug)
    {
        printf("ACPI: Destructing\n");
    }

    LinkedList_Destructor(&self->IoApicRecordPointers);
    LinkedList_Destructor(&self->InterruptSourceOverrideRecordPointers);
}

void* ACPIDriver_GetEbdaPointer(struct ACPIDriver* self)
{
    if (self->Debug) 
    {
        printf("ACPI: Getting RSDP Pointer\n");
    }
    uint16_t* ptr = (uint16_t*)ACPI_EBDA_POINTER_ADDR;
    void* vPtr = (void*)(0x0000FFFF & (uint16_t)*ptr);
    return vPtr;
}

void* ACPIDriver_FindRsdpPointer(struct ACPIDriver* self)
{
    void* currentAddr;
    void* ebdaStart = ACPIDriver_GetEbdaPointer(self);

    for (currentAddr = ebdaStart; currentAddr < ebdaStart + 1024; currentAddr+=16)
    {
        if (!strncmp((char*)currentAddr,ACPI_RSDP_HEADER_STR,8))
        {
            if (self->Debug)  
            {
               printf("ACPI: Found RSDP Header in EBDA at 0x%x\n",(uint32_t)currentAddr);
            }
            return currentAddr;
        }
    }

    ebdaStart = (void*)(((uint32_t)ebdaStart) << 4);

    for (currentAddr = ebdaStart; currentAddr < ebdaStart + 1024; currentAddr+=16)
    {
        if (!strncmp((char*)currentAddr,ACPI_RSDP_HEADER_STR,8))
        {
            if (self->Debug) 
            {
                printf("ACPI: Found RSDP Header in EBDA at 0x%x\n",(uint32_t)currentAddr);
            }
            return currentAddr;
        }
    }

    for (currentAddr = (void*)ACPI_BIOS_SEARCH_BEGIN; 
         currentAddr <= (void*)ACPI_BIOS_SEARCH_END; 
         currentAddr += (uint8_t)ACPI_BIOS_SEARCH_STEP)
    {
        if (!strncmp((char*)currentAddr,ACPI_RSDP_HEADER_STR,8))
        {
            if (self->Debug) 
            {
                printf("ACPI: Found RSDP Header in higher ram at 0x%x\n",(uint32_t)currentAddr);
            }
            return currentAddr;
        }
    }
    return 0;
}

uint8_t ACPIDriver_CheckRsdpChecksum(struct ACPIDriver* self, struct ACPI_RsdpDescriptorV1* rsdp, uint8_t version)
{
    uint32_t result = 0;
    uint8_t* headerBytes = (uint8_t*)rsdp;
    uint32_t rsdpLen = sizeof(struct ACPI_RsdpDescriptorV1);
    int i;
    for (i=0; i<rsdpLen; i++)
    {
        result += headerBytes[i];
    }
    if (self->Debug) 
    {
        printf("ACPI: V1 Checksum Result 0x%x\n",result);
    }
    result &= 0x000000FF;
    result = (result == 0x00);
    
    if (result)
    {
        if (self->Debug) 
        {
            printf("ACPI: V1 Checksum Valid\n");
        }
        if (version)
        {
            struct ACPI_RsdpDescriptorV2* v2 = (struct ACPI_RsdpDescriptorV2*)rsdp;
            if (self->Debug) 
            {
                printf("ACPI: Version 2 > check not implemented\n");
            }
        }
    }
    else
    {
        if (self->Debug) 
        {
            printf("ACPI: Checksum Invalid\n");
        }
    }

    return result;
}


uint8_t ACPIDriver_CheckSDTChecksum(struct ACPIDriver* self, struct ACPI_SDTHeader *tableHeader)
{
    unsigned char sum = 0;
    if (self->Debug) 
    {
        printf("ACPI: Checking SDT Checksum for table of length %d\n", tableHeader->Length);
    }
    if (tableHeader->Length > 0xFFFF)
    {
        if (self->Debug) 
        {
            printf("ACPI: table is too long for me %d\n",0xFFFF);
        }
        return 0;
    }
    for (int i = 0; i < tableHeader->Length; i++)
    {
        sum += ((char *) tableHeader)[i];
    }
 
    return sum == 0;
}


void* ACPIDriver_FindSDTBySignature(struct ACPIDriver* self, const char* sig, void* RootSDT, uint8_t version)
{
    void* rsdt = 0;
    int entries = 0;
    uint32_t  headerLen = sizeof(struct ACPI_SDTHeader);

    if (version)
    {
        /* Version 2.0 > */
        struct ACPI_XSDT* rsdt = (struct ACPI_XSDT*)RootSDT;
        entries = (rsdt->h.Length - headerLen) / 8;
        if (self->Debug) 
        {
            printf("ACPI: V2 - There are %d entries in length %d, header %d\n",
                entries, rsdt->h.Length,headerLen);
        }
        uint64_t* start = (uint64_t*)&rsdt->PointerToOtherSDT;

        for (int i = 0; i < entries; i++)
        {
            struct ACPI_SDTHeader *h =  (struct ACPI_SDTHeader*) (uint32_t)start[i];
            if (self->Debug) 
            {
                printf("ACPI: Found signature (%c%c%c%c)\n",
                    h->Signature[0],
                    h->Signature[1],
                    h->Signature[2],
                    h->Signature[3]
                );
            }
            if (!strncmp(h->Signature, sig, 4))
            {
                if (self->Debug) 
                {
                    printf("ACPI: at 0x%x\n",h);
                }
                if (!ACPIDriver_CheckSDTChecksum(self, h))
                {
                    if (self->Debug) 
                    {
                        printf("ACPI: SDT Checksum is invalid\n");
                    }
                }
                else
                {
                    if (self->Debug) 
                    {
                        printf("ACPI: SDT Checksum is valid\n");
                    }
                    return (void *) h;
                }
            }
        }
    }
    else
    {
        /* Version 1.0 */
        struct ACPI_RSDT *rsdt = (struct ACPI_RSDT*) RootSDT;
        entries = (rsdt->h.Length - headerLen) / 4;
        if (self->Debug) 
        {
            printf(
                "ACPI: V1 - There are %d entries in length %d - header %d = %d\n",
                entries,rsdt->h.Length,headerLen, rsdt->h.Length-headerLen
            );
        }

        struct ACPI_SDTHeader** start = (struct ACPI_SDTHeader**) &rsdt->PointerToOtherSDT;
        for (int i = 0; i < entries; i++)
        {
            struct ACPI_SDTHeader *h =  start[i];
            if (self->Debug) 
            {
                printf("ACPI: Found signature (%c%c%c%c) at 0x%x\n",
                    h->Signature[0],
                    h->Signature[1],
                    h->Signature[2],
                    h->Signature[3],
                    h
                );
            }
            if (!strncmp(h->Signature, sig, 4))
            {
                if (self->Debug) 
                {
                    printf("ACPI: at 0x%x\n",h);
                }

                if (!ACPIDriver_CheckSDTChecksum(self, h))
                {
                    if (self->Debug)  
                    {
                       printf("ACPI: SDT Checksum is invalid\n");
                    }
                }
                else
                {
                    if (self->Debug)  
                    {
                        printf("ACPI: SDT Checksum is valid\n");
                    }
                    return (void *) h;
                }
            }
        }
    }

    if (self->Debug) 
    {
        printf("ACPI: Could not find %s Signature\n",sig);
    }
    // No FACP found
    return 0;
}

void ACPIDriver_SetFacsPointer(struct ACPIDriver* self, uint32_t ptr)
{
    self->FacsPointer = ptr;
    if (self->Debug) 
    {
        printf("ACPI: Set FACS Pointer 0x%x\n",self->FacsPointer);
    }
}

void ACPIDriver_SetSciInterrupt(struct ACPIDriver* self, uint16_t sci)
{
    self->SciInterrupt = sci;
    if (self->Debug) 
    {
        printf("ACPI: Set SCI Interrupt 0x%x\n",self->SciInterrupt);
    }
}

void ACPIDriver_SetDsdtPointer(struct ACPIDriver* self, uint32_t dsdt)
{
    self->DsdtPointer = dsdt;
    if (self->Debug) 
    {
        printf("ACPI: Set DSDT pointer 0x%x\n",self->DsdtPointer);
    }
}

void APCI_SetDebug(struct ACPIDriver* self, uint8_t debug)
{
    self->Debug = debug;
}

void ACPIDriver_ProcessMADT(struct ACPIDriver* self, struct ACPI_MADT* madt)
{
    if (madt->Flags == 0x01)
    {
        if (self->Debug)
        {
            printf("ACPI: Found 8259 in MADT, disabling interrupts\n");
            PS2Driver_WaitForKeyPress("ACPI Pause");
        }
	    //Interrupt_SetMask_PIC1(self->Interrupt, 0xFD);
	    //Interrupt_SetMask_PIC2(self->Interrupt, 0xFF);
    }

    uint32_t recordsBegin = (uint32_t)&madt->RecordsStart;
    uint32_t recordsEnd = ((uint32_t)madt) + madt->h.Length;
    uint32_t current = recordsBegin;

    uint8_t error = 0;
    while (current < recordsEnd && !error)
    {
        struct ACPI_MADTRecordBase* currentRecord = (struct ACPI_MADTRecordBase*)current;

        switch (currentRecord->RecordType)
        {
            case ACPI_MADT_RECORD_TYPE_PROC_LOCAL_APIC:
            {
                if (self->Debug)
                {
                    printf("ACPI: MADT Record Found: Processor Local Apic\n");
                    PS2Driver_WaitForKeyPress("ACPI Pause");
                }
                struct ACPI_MADTRecordApic* apic = (struct ACPI_MADTRecordApic*)current;
                current += apic->Base.Length;
                break;
            }
            case ACPI_MADT_RECORD_TYPE_IOAPIC:
            {
                if (self->Debug)
                {
                    printf("ACPI: MADT Record Found: I/O Apic\n");
                    PS2Driver_WaitForKeyPress("ACPI Pause");
                }
                struct ACPI_MADTRecordIoApic* ioapic = (struct ACPI_MADTRecordIoApic*)current;
                LinkedList_PushBack(&self->IoApicRecordPointers, ioapic);
                current += ioapic->Base.Length;
                break;
            }
            case ACPI_MADT_RECORD_TYPE_ISO:
            {
                if (self->Debug)
                {
                    printf("ACPI: MADT Record Found: Interrupt Source Override\n");
                    PS2Driver_WaitForKeyPress("ACPI Pause");
                }

                struct ACPI_MADTRecordInputSourceOverride* intSrcOvr = 0;
                intSrcOvr = (struct ACPI_MADTRecordInputSourceOverride*)current;
                LinkedList_PushBack(&self->InterruptSourceOverrideRecordPointers,intSrcOvr);
                current += intSrcOvr->Base.Length;
                break;
            }
            case ACPI_MADT_RECORD_TYPE_NMI:
            {
                if (self->Debug)
                {
                    printf("ACPI: MADT Record Found: Non-Maskable Interrupt\n");
                    PS2Driver_WaitForKeyPress("ACPI Pause");
                }

                struct ACPI_MADTRecordNonMaskableInterrupt* nmi = (struct ACPI_MADTRecordNonMaskableInterrupt*)current;
                current += nmi->Base.Length;
                break;
            }
            case ACPI_MADT_RECORD_TYPE_LOCAL_APIC_OVR:
            {
                if (self->Debug)
                {
                    printf("ACPI: MADT Record Found: Local Apic Address Override\n");
                    PS2Driver_WaitForKeyPress("ACPI Pause");
                }

                struct ACPI_MADTRecordLocalApicAddressOverride* localApicAddrOvr = (struct ACPI_MADTRecordLocalApicAddressOverride*)current;
                current += localApicAddrOvr->Base.Length;
                break;
            }
            default:
            {
                if (self->Debug)
                {
                    printf("ACPI: Error unrecognised MADT Record type 0x%x\n",currentRecord->RecordType);
                    PS2Driver_WaitForKeyPress("ACPI Pause");
                }
                error = 1;
                break;
            }
        }
    
    }
    if (self->Debug)
    {
        ACPIDriver_DebugMADT(self);
    }
}

struct ACPI_IoApic ACPIDriver_GenerateIoApic(struct ACPIDriver* self, struct ACPI_MADTRecordIoApic* ioapic)
{
    struct ACPI_IoApic device;

    *(volatile uint32_t*)(ioapic->IoApicAddress) = 0;
    device.Id = *(volatile uint32_t*)(ioapic->IoApicAddress + 0x10);

    *(volatile uint32_t*)(ioapic->IoApicAddress) = 1;
    device.Version = *(volatile uint32_t*)(ioapic->IoApicAddress + 0x10);
    device.IrqRedirectEntries = (device.Version >> 16) + 1;// cast to uint8_t occuring ok!

    *(volatile uint32_t*)(ioapic->IoApicAddress) = 2;
    device.Arbitration = *(volatile uint32_t*)(ioapic->IoApicAddress + 0x10);

    int i, j;

    for (i=0, j=0x10; i<device.IrqRedirectEntries; i++)
    {
        *(volatile uint32_t*)(ioapic->IoApicAddress) = j++;
        device.IrqRedirections[i].lowerDword = *(volatile uint32_t*)(ioapic->IoApicAddress + 0x10);

        *(volatile uint32_t*)(ioapic->IoApicAddress) = j++;
        device.IrqRedirections[i].upperDword = *(volatile uint32_t*)(ioapic->IoApicAddress + 0x10);
    }
    return device;
}

void ACPIDriver_DriverDebugIoApic(struct ACPIDriver* self, struct ACPI_MADTRecordIoApic* record)
{
    struct ACPI_IoApic device = ACPIDriver_GenerateIoApic(self, record);
    if (self->Debug)
    {
        printf("ACPI: I/0 Apic Record\n",record->IoApicId);
        printf("\tAddress: 0x%x\n",record->IoApicAddress);
        printf("\tGSI Base: 0x%x\n",record->GlobalSystemInterruptBase);
        printf("\tID: 0x%x\n",device.Id);
        printf("\tVersion: 0x%x\n",device.Version);
        printf("\tArbitration: 0x%x\n",device.Arbitration);
        printf("\tRedirections: %d\n",device.IrqRedirectEntries);

        int i;
        for (i=0; i<device.IrqRedirectEntries; i++)
        {
            union ACPI_IoApicRedirectionEntry r = device.IrqRedirections[i];
            printf("\t\t- %d ----------------------------------------\n",i);
            printf("\t\t\tLow 0x%x High 0x%x\n",r.lowerDword, r.upperDword);
            printf("\t\t\tVector 0x%x\n",r.vector);
            printf("\t\t\tDelivery Mode 0x%x\n", r.delvMode);
            printf("\t\t\tDestination Mode 0x%x\n", r.destMode);
            printf("\t\t\tDelivery Status 0x%x\n", r.delvStatus);
            printf("\t\t\tPin Polarity 0x%x\n", r.pinPolarity);
            printf("\t\t\tRemote IRR 0x%x\n", r.remoteIRR);
            printf("\t\t\tTrigger Mode 0x%x\n", r.triggerMode);
            printf("\t\t\tMask 0x%x\n", r.mask);
            printf("\t\t\tDestination 0x%x\n", r.destination);
        }

    }
}

void ACPIDriver_DebugInterruptSourceOverride(struct ACPIDriver* self, struct ACPI_MADTRecordInputSourceOverride* record)
{
    if (self->Debug)
    {
        printf("ACPI: Interrupt Source Override\n");
        printf("\tBus Source: 0x%x\n",record->BusSource);
        printf("\tIRQ Source: 0x%x\n",record->IrqSource);
        printf("\tGIS: 0x%x\n",record->GlobalSystemInterrupt);
        printf("\tFlags: 0x%x\n",record->Flags);
        PS2Driver_WaitForKeyPress("ACPI Pause");
    }
}

void ACPIDriver_DebugMADT(struct ACPIDriver* self)
{
    int nElements, i;
    /*
    for (nElements = LinkedList_Size(ACPIDriver_IoApicRecordPointers), i=0; i<nElements; i++)
    {
        ACPI_MADTRecordIoApic* ioapic = LinkedList_At(ACPIDriver_IoApicRecordPointers,i);
        self->DebugIoApic(ioapic);
    }
    */

    for (nElements = LinkedList_Size(&self->InterruptSourceOverrideRecordPointers), i=0; i<nElements; i++)
    {
        struct ACPI_MADTRecordInputSourceOverride* iso = LinkedList_At(&self->InterruptSourceOverrideRecordPointers, i);
        ACPIDriver_DebugInterruptSourceOverride(self, iso);
    }

}