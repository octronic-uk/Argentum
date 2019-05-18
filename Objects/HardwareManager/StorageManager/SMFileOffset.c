#include "SMFileOffset.h"
#include <stdio.h>

void SMFileOffset_Debug(SMFileOffset* self)
{
    printf(
        "SMFileOffset:\n"
        "\tSectorsPerCluster ...... 0x%x\n"
        "\tBytesPerSector ......... 0x%x\n"
        "\tOffsetInSectors ........ 0x%x\n"
        "\tCluster ................ 0x%x\n"
        "\tSectorOffsetInCluster .. 0x%x\n"
        "\tByteOffsetIntoSector ... 0x%x\n",
        self->SectorsPerCluster,
        self->BytesPerSector,
        self->Cluster,
        self->SectorOffsetInCluster,
        self->ByteOffsetIntoSector
    );
}