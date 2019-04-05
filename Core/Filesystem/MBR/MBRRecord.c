#include <Filesystem/MBR/MBRRecord.h>
#include <stdio.h>
#include <stddef.h>

void MBRRecord_Debug(struct MBRRecord* record)
{
    printf("MBR: Record (%d bytes)\n",sizeof(struct MBRRecord));
    printf("\tBootstrapCode1Area: 0x%x (%d)\n", record->BootstrapCodeArea1, offsetof(struct MBRRecord,BootstrapCodeArea1));
    printf("\tBlank1: 0x%x (%d)\n", record->Blank1, offsetof(struct MBRRecord,Blank1));
    printf("\tOriginalPhysicalDrive: 0x%x (%d)\n", record->OriginalPhysicalDrive, offsetof(struct MBRRecord,OriginalPhysicalDrive));
    printf("\tTimestampSeconds: 0x%x (%d)\n", record->TimestampSeconds, offsetof(struct MBRRecord,TimestampSeconds));
    printf("\tTimestampMinutes: 0x%x (%d)\n", record->TimestampMinutes, offsetof(struct MBRRecord,TimestampMinutes));
    printf("\tTimestampHours: 0x%x (%d)\n", record->TimestampHours, offsetof(struct MBRRecord,TimestampHours));
    printf("\tBootatrapCodeArea2: 0x%x (%d)\n", record->BootstrapCodeArea2, offsetof(struct MBRRecord,BootstrapCodeArea2));
    printf("\tDiskSignature: 0x%x (%d)\n", record->DiskSignature, offsetof(struct MBRRecord,DiskSignature));
    printf("\tCopyProtected: 0x%x (%d)\n", record->CopyProtected, offsetof(struct MBRRecord,CopyProtected));

    MBRPartitionEntry_Debug(1, &record->PartitionEntry1);
    MBRPartitionEntry_Debug(2, &record->PartitionEntry2);
    MBRPartitionEntry_Debug(3, &record->PartitionEntry3);
    MBRPartitionEntry_Debug(4, &record->PartitionEntry4);

    printf("\tBootSignature: 0x%x (%d)\n", record->BootSignature, offsetof(struct MBRRecord,BootSignature));
}

void MBRPartitionEntry_Debug(int index, struct MBRPartitionEntry* pe)
{
    printf("\tPartitionEntry%d:\n", index,sizeof(struct MBRPartitionEntry));
    printf("\t\tStatus: 0x%x\n",pe->Status);

    struct MBRChsAddress first, last;
    MBRChsAddress_From3b(&first, &pe->CHSFirstSector);
    MBRChsAddress_From3b(&last, &pe->CHSLastSector);

    MBRChsAddress_Debug("CHS First Sector", first);
    printf("\t\tPartition Type: 0x%x\n",pe->PartitionType);
    MBRChsAddress_Debug("CHS Last Sector", last);
    printf("\t\tLBA First Sector: 0x%x\n",pe->LBAFirstSector);
    printf("\t\tSector Count: 0x%x\n",pe->SectorCount);
}