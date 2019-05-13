#include "MBRRecord.h"

#include <stdio.h>
#include <stddef.h>
#include <Drivers/PS2/PS2Driver.h>

void MBRRecord_Debug(MBRRecord* record)
{
    printf("MBR: Record (%d bytes)\n",sizeof(MBRRecord));
    printf("\tBootstrapCode1Area: 0x%x (%d)\n", record->BootstrapCodeArea1, offsetof(MBRRecord,BootstrapCodeArea1));
    printf("\tBlank1: 0x%x (%d)\n", record->Blank1, offsetof(MBRRecord,Blank1));
    printf("\tOriginalPhysicalDrive: 0x%x (%d)\n", record->OriginalPhysicalDrive, offsetof(MBRRecord,OriginalPhysicalDrive));
    printf("\tTimestampSeconds: 0x%x (%d)\n", record->TimestampSeconds, offsetof(MBRRecord,TimestampSeconds));
    printf("\tTimestampMinutes: 0x%x (%d)\n", record->TimestampMinutes, offsetof(MBRRecord,TimestampMinutes));
    printf("\tTimestampHours: 0x%x (%d)\n", record->TimestampHours, offsetof(MBRRecord,TimestampHours));
    printf("\tBootatrapCodeArea2: 0x%x (%d)\n", record->BootstrapCodeArea2, offsetof(MBRRecord,BootstrapCodeArea2));
    printf("\tDiskSignature: 0x%x (%d)\n", record->DiskSignature, offsetof(MBRRecord,DiskSignature));
    printf("\tCopyProtected: 0x%x (%d)\n", record->CopyProtected, offsetof(MBRRecord,CopyProtected));

    MBRPartitionEntry_Debug(&record->PartitionEntry1, 1);
    MBRPartitionEntry_Debug(&record->PartitionEntry2, 2);
    MBRPartitionEntry_Debug(&record->PartitionEntry3, 3);
    MBRPartitionEntry_Debug(&record->PartitionEntry4, 4);

    printf("\tBootSignature: 0x%x (%d)\n", record->BootSignature, offsetof(MBRRecord,BootSignature));
}