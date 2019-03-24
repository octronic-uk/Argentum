#include "ACPI.h"

#include <stdio.h>
#include <string.h>

#include <Drivers/IO/IO.h>

uint8_t ACPI_Debug = 0;

uint32_t ACPI_FacsPointer = 0;
uint32_t ACPI_DsdtPointer = 0;
uint16_t ACPI_SciInterrupt = 0;

void ACPI_Constructor()
{
    printf("ACPI: Constructing\n");
    void* rsdpPointer = ACPI_FindRsdpPointer();

    printf("ACPI: RSDP Pointer found: 0x%x\n",rsdpPointer);
    ACPI_RsdpDescriptorV1* v1Header = (ACPI_RsdpDescriptorV1*)rsdpPointer;

    if (ACPI_Debug) printf("ACPI: OEM ID (%s)\n",v1Header->OEMID);

    // ACPI V1
    uint8_t version  = v1Header->Revision;
    if (version == 0)
    {
        if (ACPI_Debug) printf("ACPI: Version == 1.0\n");
    }
    // ACPI v2 to v6.1
    else if (version == 2)
    {
        if (ACPI_Debug) printf("ACPI: Version >= 2.0\n");
    }
    else
    {
        printf("ACPI: Version is INVALID\n");
        return;
    }
    if (!ACPI_CheckRsdpChecksum(v1Header,version))
    {
        printf("ACPI: RSDT Checksum is invalid\n");
        return;
    }

    void* rsdt = v1Header->RsdtAddress;
    if (ACPI_Debug) printf("ACPI: using RsdtAddress 0x%x\n",rsdt);
    ACPI_FADT* facp = ACPI_FindSDTBySignature(ACPI_SIG_FACP, rsdt,version);
    if (facp)
    {
       if (ACPI_Debug) printf("ACPI: Found FACP\n");
       ACPI_SetFacsPointer(facp->FirmwareCtrl);
       ACPI_SetDsdtPointer(facp->Dsdt);
       ACPI_SetSciInterrupt(facp->SCI_Interrupt);
    }
    
}

void ACPI_Destructor()
{
    printf("ACPI: Destructing\n");
}

void* ACPI_GetEbdaPointer()

{
    if (ACPI_Debug) printf("ACPI: Getting RSDP Pointer\n");
    uint16_t* ptr = (uint16_t*)ACPI_EBDA_POINTER_ADDR;
    void* vPtr = (void*)(0x0000FFFF & (uint16_t)*ptr);
    return vPtr;
}

void* ACPI_FindRsdpPointer()
{
    void* currentAddr;
    void* ebdaStart = ACPI_GetEbdaPointer();

    for (currentAddr = ebdaStart; currentAddr < ebdaStart + 1024; currentAddr+=16)
    {
        if (!strncmp((char*)currentAddr,ACPI_RSDP_HEADER_STR,8))
        {
           if (ACPI_Debug)  printf("ACPI: Found RSDP Header in EBDA at 0x%x\n",(uint32_t)currentAddr);
            return currentAddr;
        }
    }

    ebdaStart = (void*)(((uint32_t)ebdaStart) << 4);

    for (currentAddr = ebdaStart; currentAddr < ebdaStart + 1024; currentAddr+=16)
    {
        if (!strncmp((char*)currentAddr,ACPI_RSDP_HEADER_STR,8))
        {
            if (ACPI_Debug) printf("ACPI: Found RSDP Header in EBDA at 0x%x\n",(uint32_t)currentAddr);
            return currentAddr;
        }
    }

    for (currentAddr = (void*)ACPI_BIOS_SEARCH_BEGIN; 
         currentAddr <= (void*)ACPI_BIOS_SEARCH_END; 
         currentAddr += (uint8_t)ACPI_BIOS_SEARCH_STEP)
    {
        if (!strncmp((char*)currentAddr,ACPI_RSDP_HEADER_STR,8))
        {
            if (ACPI_Debug) printf("ACPI: Found RSDP Header in higher ram at 0x%x\n",(uint32_t)currentAddr);
            return currentAddr;
        }
    }
    return 0;
}

uint8_t ACPI_CheckRsdpChecksum(ACPI_RsdpDescriptorV1* rsdp, uint8_t version)
{
    uint32_t result = 0;
    uint8_t* headerBytes = (uint8_t*)rsdp;
    uint32_t rsdpLen = sizeof(ACPI_RsdpDescriptorV1);
    int i;
    for (i=0; i<rsdpLen; i++)
    {
        result += headerBytes[i];
    }
    if (ACPI_Debug) printf("ACPI: V1 Checksum Result 0x%x\n",result);
    result &= 0x000000FF;
    result = (result == 0x00);
    
    if (result)
    {
        if (ACPI_Debug) printf("ACPI: V1 Checksum Valid\n");
        if (version)
        {
            ACPI_RsdpDescriptorV2* v2 = (ACPI_RsdpDescriptorV2*)rsdp;
            if (ACPI_Debug) printf("ACPI: Version 2 > check not implemented\n");
        }
    }
    else
    {
        if (ACPI_Debug) printf("ACPI: Checksum Invalid\n");
    }

    return result;
}


uint8_t ACPI_CheckSDTChecksum(ACPI_SDTHeader *tableHeader)
{
    unsigned char sum = 0;
    if (ACPI_Debug) printf("ACPI: Checking SDT Checksum for table of length %d\n", tableHeader->Length);
    if (tableHeader->Length > 0xFFFF)
    {
        if (ACPI_Debug) printf("ACPI: table is too long for me %d\n",0xFFFF);
        return 0;
    }
    for (int i = 0; i < tableHeader->Length; i++)
    {
        sum += ((char *) tableHeader)[i];
    }
 
    return sum == 0;
}


void* ACPI_FindSDTBySignature(const char* sig, void* RootSDT, uint8_t version)
{
    void* rsdt = 0;
    int entries = 0;
    uint32_t  headerLen = sizeof(ACPI_SDTHeader);

    if (version)
    {
        /* Version 2.0 > */
        ACPI_XSDT* rsdt = (ACPI_XSDT*)RootSDT;
        entries = (rsdt->h.Length - headerLen) / 8;
        if (ACPI_Debug) 
        {
            printf(
                "ACPI: V2 - There are %d entries in length %d, header %d\n",
                entries,rsdt->h.Length,headerLen
            );
        }
        for (int i = 0; i < entries; i++)
        {
            ACPI_SDTHeader *h =  (ACPI_SDTHeader*)rsdt->PointerToOtherSDT+i*8;
            if (ACPI_Debug) printf("ACPI: Found signature (%c%c%c%c)\n",
                h->Signature[0],
                h->Signature[1],
                h->Signature[2],
                h->Signature[3]
            );
            if (!strncmp(h->Signature, sig, 4))
            {
                if (ACPI_Debug) printf("ACPI: at 0x%x\n",h);
                if (!ACPI_CheckSDTChecksum(h))
                {
                    if (ACPI_Debug) printf("ACPI: SDT Checksum is invalid\n");
                }
                else
                {
                    if (ACPI_Debug) printf("ACPI: SDT Checksum is valid\n");
                    return (void *) h;
                }
            }
        }
    }
    else
    {
        /* Version 1.0 */
        ACPI_RSDT *rsdt = (ACPI_RSDT*) RootSDT;
        entries = (rsdt->h.Length - headerLen) / 4;
        if (ACPI_Debug) printf(
            "ACPI: V1 - There are %d entries in length %d - header %d = %d\n",
            entries,rsdt->h.Length,headerLen, rsdt->h.Length-headerLen
        );

        for (int i = 0; i < entries; i++)
        {
            ACPI_SDTHeader *h =  (ACPI_SDTHeader*)rsdt->PointerToOtherSDT+i*4;
            if (ACPI_Debug) printf("ACPI: Found signature (%c%c%c%c)\n",
                h->Signature[0],
                h->Signature[1],
                h->Signature[2],
                h->Signature[3]
            );
            if (!strncmp(h->Signature, sig, 4))
            {
                if (ACPI_Debug) printf("ACPI: at 0x%x\n",h);

                if (!ACPI_CheckSDTChecksum(h))
                {
                   if (ACPI_Debug)  printf("ACPI: SDT Checksum is invalid\n");
                }
                else
                {
                    if (ACPI_Debug)  printf("ACPI: SDT Checksum is valid\n");
                    return (void *) h;
                }
            }
        }
    }

    if (ACPI_Debug) printf("ACPI: Could not find %s Signature\n",sig);
    // No FACP found
    return 0;
}

void ACPI_SetFacsPointer(uint32_t ptr)
{
    ACPI_FacsPointer = ptr;
    if (ACPI_Debug) printf("ACPI: Set FACS Pointer 0x%x\n",ACPI_FacsPointer);
}

void ACPI_SetSciInterrupt(uint16_t sci)
{
    ACPI_SciInterrupt = sci;
    if (ACPI_Debug) printf("ACPI: Set SCI Interrupt 0x%x\n",ACPI_SciInterrupt);
}

void ACPI_SetDsdtPointer(uint32_t dsdt)
{
    ACPI_DsdtPointer = dsdt;
    if (ACPI_Debug) printf("ACPI: Set DSDT pointer 0x%x\n",ACPI_DsdtPointer);
}

void APCI_SetDebug(uint8_t debug)
{
    ACPI_Debug = debug;
}