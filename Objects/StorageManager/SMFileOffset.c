#include "SMFileOffset.h"
#include <stdio.h>

void SMFileOffset_Debug(SMFileOffset* self)
{
    printf(
        "SMFileOffset:\n"
        "\tSectorsPerCluster ...... 0x%x\n"
        "\tSectorSize ............. 0x%x\n"
        "\tOffsetInSectors ........ 0x%x\n"
        "\tCluster ................ 0x%x\n"
        "\tSectorInCluster ........ 0x%x\n"
        "\tByteOffsetIntoSector ... 0x%x\n",
        self->SectorsPerCluster,
        self->SectorSize,
        self->OffsetInSectors,
        self->Cluster,
        self->SectorInCluster,
        self->ByteOffsetIntoSector
    );
}