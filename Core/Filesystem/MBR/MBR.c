#include "MBR.h"

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <Drivers/ATA/ATA.h>
#include <Memory/Memory.h>


bool MbrConstructor(struct MBR* self, struct ATA* ata, uint8_t ata_device_id)
{
    self->ATA = ata;
    self->IDEDevice = &self->ATA->IDEDevices[ata_device_id];

    printf("MBR: Constructing Record for device: %s\n",self->IDEDevice->model);
	if (self->IDEDevice->reserved != 1)
	{
		printf("MBR: Error - Constructing Record Failed: No Device\n");
		return false;
	}

    ATA_IDEAccess(self->ATA, 0,ata_device_id,0,1,0,(uint32_t)&self->Record);

    if (self->Debug)
    {
        MbrRecord_Debug(&self->Record);
    }
    return true;
}

void MbrRecord_Debug(struct MbrRecord* record)
{
    printf("MBR: Record (%d bytes)\n",sizeof(struct MbrRecord));
    printf("\tBootstrapCode1Area: 0x%x (%d)\n", record->BootstrapCodeArea1, offsetof(struct MbrRecord,BootstrapCodeArea1));
    printf("\tBlank1: 0x%x (%d)\n", record->Blank1, offsetof(struct MbrRecord,Blank1));
    printf("\tOriginalPhysicalDrive: 0x%x (%d)\n", record->OriginalPhysicalDrive, offsetof(struct MbrRecord,OriginalPhysicalDrive));
    printf("\tTimestampSeconds: 0x%x (%d)\n", record->TimestampSeconds, offsetof(struct MbrRecord,TimestampSeconds));
    printf("\tTimestampMinutes: 0x%x (%d)\n", record->TimestampMinutes, offsetof(struct MbrRecord,TimestampMinutes));
    printf("\tTimestampHours: 0x%x (%d)\n", record->TimestampHours, offsetof(struct MbrRecord,TimestampHours));
    printf("\tBootatrapCodeArea2: 0x%x (%d)\n", record->BootstrapCodeArea2, offsetof(struct MbrRecord,BootstrapCodeArea2));
    printf("\tDiskSignature: 0x%x (%d)\n", record->DiskSignature, offsetof(struct MbrRecord,DiskSignature));
    printf("\tCopyProtected: 0x%x (%d)\n", record->CopyProtected, offsetof(struct MbrRecord,CopyProtected));

    MbrPartitionEntry_Debug(1, &record->PartitionEntry1);
    MbrPartitionEntry_Debug(2, &record->PartitionEntry2);
    MbrPartitionEntry_Debug(3, &record->PartitionEntry3);
    MbrPartitionEntry_Debug(4, &record->PartitionEntry4);

    printf("\tBootSignature: 0x%x (%d)\n", record->BootSignature, offsetof(struct MbrRecord,BootSignature));
}

void MbrPartitionEntry_Debug(int index, struct MbrPartitionEntry* pe)
{
    printf("\tPartitionEntry%d:\n", index,sizeof(struct MbrPartitionEntry));
    printf("\t\tStatus: 0x%x\n",pe->Status);

    struct MbrChsAddress first, last;
    MbrChsAddress_From3b(&first, &pe->CHSFirstSector);
    MbrChsAddress_From3b(&last, &pe->CHSLastSector);

    MbrChsAddress_Debug("CHS First Sector", first);
    printf("\t\tPartition Type: 0x%x\n",pe->PartitionType);
    MbrChsAddress_Debug("CHS Last Sector", last);
    printf("\t\tLBA First Sector: 0x%x\n",pe->LBAFirstSector);
    printf("\t\tSector Count: 0x%x\n",pe->SectorCount);
}

void MbrChsAddress_Debug(const char* name, struct MbrChsAddress addr)
{
    printf("\t\t%s:\n", name, sizeof (struct MbrChsAddress));
    printf("\t\t\tCylinder: 0x%x\n", addr.Cylinder);
    printf("\t\t\tHead: 0x%x\n", addr.Head);
    printf("\t\t\tSector: 0x%x\n", addr.Sector);
}

/*
    CHS0: head
    CHS1: sector in bits 5–0; bits 7–6 are high bits of cylinder
    CHS2: bits 7–0 of cylinder
*/
void MbrChsAddress_From3b(struct MbrChsAddress* self, struct MbrChsAddress3b* data)
{
    memset(&self,0,sizeof(struct MbrChsAddress));
    self->Head = data->CHS0;
    self->Sector = data->CHS1 & 0x3F;
    self->Cylinder = (data->CHS1 & 0xC0) << 2;
    self->Cylinder |= data->CHS2;
}