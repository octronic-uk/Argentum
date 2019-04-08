#include "MBRPartitionEntry.h"

#include <stdio.h>
#include "MBRChsAddress.h"
#include <Drivers/PS2/PS2Driver.h>

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
    printf("\t\tSectors In Partition: 0x%x\n",pe->SectorsInPartition);

    PS2Driver_WaitForKeyPress();
}