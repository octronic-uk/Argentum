#include "MBR.h"

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <Drivers/ATA/ATA.h>
#include <Memory/Memory.h>

bool MBR_Debug = false;

MBR_Record* MBR_RecordConstructor(uint8_t ata_device_id)
{
    ATA_IDEDevice* device = &ATA_IDEDevices[ata_device_id];
    printf("MBR: Constructing Record for device: %s\n",device->model);
	if (device->reserved != 1)
	{
		printf("MBR: Error - Constructing Record Failed: No Device\n");
		return 0;
	}

    MBR_Record* record = Memory_Allocate(sizeof(MBR_Record));

    if (!record)
    {
        printf("MBR: Error - Could not allocate memory\n");
        return 0;
    }

    ATA_IDEAccess(0,ata_device_id,0,1,0,(uint32_t)record);

    if (MBR_Debug)
    {
        MBR_DebugRecord(record);
    }

    return record;
}

bool MBR_RecordPreallocatedConstructor(MBR_Record* record, uint8_t ata_device_id)
{
    ATA_IDEDevice* device = &ATA_IDEDevices[ata_device_id];
    printf("MBR: Constructing Record for device: %s\n",device->model);
	if (device->reserved != 1)
	{
		printf("MBR: Error - Constructing Record Failed: No Device\n");
        return false;
	}

    if (!record)
    {
        printf("MBR: Error - Record not allocated in memory\n");
        return false;
    }

    ATA_IDEAccess(0,ata_device_id,0,1,0,(uint32_t)record);

    if (MBR_Debug)
    {
        MBR_DebugRecord(record);
    }
}

void MBR_DebugRecord(MBR_Record* record)
{
    printf("MBR: Record (%d bytes)\n",sizeof(MBR_Record));
    printf("\tBootstrapCode1Area: 0x%x (%d)\n", record->BootstrapCodeArea1, offsetof(MBR_Record,BootstrapCodeArea1));
    printf("\tBlank1: 0x%x (%d)\n", record->Blank1, offsetof(MBR_Record,Blank1));
    printf("\tOriginalPhysicalDrive: 0x%x (%d)\n", record->OriginalPhysicalDrive, offsetof(MBR_Record,OriginalPhysicalDrive));
    printf("\tTimestampSeconds: 0x%x (%d)\n", record->TimestampSeconds, offsetof(MBR_Record,TimestampSeconds));
    printf("\tTimestampMinutes: 0x%x (%d)\n", record->TimestampMinutes, offsetof(MBR_Record,TimestampMinutes));
    printf("\tTimestampHours: 0x%x (%d)\n", record->TimestampHours, offsetof(MBR_Record,TimestampHours));
    printf("\tBootatrapCodeArea2: 0x%x (%d)\n", record->BootstrapCodeArea2, offsetof(MBR_Record,BootstrapCodeArea2));
    printf("\tDiskSignature: 0x%x (%d)\n", record->DiskSignature, offsetof(MBR_Record,DiskSignature));
    printf("\tCopyProtected: 0x%x (%d)\n", record->CopyProtected, offsetof(MBR_Record,CopyProtected));

    MBR_DebugPartitionEntry(1, &record->PartitionEntry1);
    MBR_DebugPartitionEntry(2, &record->PartitionEntry2);
    MBR_DebugPartitionEntry(3, &record->PartitionEntry3);
    MBR_DebugPartitionEntry(4, &record->PartitionEntry4);

    printf("\tBootSignature: 0x%x (%d)\n", record->BootSignature, offsetof(MBR_Record,BootSignature));
}

void MBR_DebugPartitionEntry(int index, MBR_PartitionEntry* pe)
{
    printf("\tPartitionEntry%d:\n", index,sizeof(MBR_PartitionEntry));
    printf("\t\tStatus: 0x%x\n",pe->Status);
    MBR_DebugCHSAddress("CHS First Sector", MBR_ConvertCHSAddress3b(&pe->CHSFirstSector));
    printf("\t\tPartition Type: 0x%x\n",pe->PartitionType);
    MBR_DebugCHSAddress("CHS Last Sector", MBR_ConvertCHSAddress3b(&pe->CHSLastSector));
    printf("\t\tLBA First Sector: 0x%x\n",pe->LBAFirstSector);
    printf("\t\tSector Count: 0x%x\n",pe->SectorCount);
}

void MBR_DebugCHSAddress(const char* name, MBR_CHSAddress addr)
{
    printf("\t\t%s:\n", name, sizeof (MBR_CHSAddress));
    printf("\t\t\tCylinder: 0x%x\n", addr.Cylinder);
    printf("\t\t\tHead: 0x%x\n", addr.Head);
    printf("\t\t\tSector: 0x%x\n", addr.Sector);
}

/*
    CHS0: head
    CHS1: sector in bits 5–0; bits 7–6 are high bits of cylinder
    CHS2: bits 7–0 of cylinder
*/
MBR_CHSAddress MBR_ConvertCHSAddress3b(MBR_CHSAddress3b* data)
{
    MBR_CHSAddress out;
    memset(&out,0,sizeof(MBR_CHSAddress));
    out.Head = data->CHS0;
    out.Sector = data->CHS1 & 0x3F;
    out.Cylinder = (data->CHS1 & 0xC0) << 2;
    out.Cylinder |= data->CHS2;
    return out;
}