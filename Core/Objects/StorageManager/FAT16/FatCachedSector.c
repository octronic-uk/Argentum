#include "FatCachedSector.h"

#include <stdio.h>
#include <string.h>

#include "FatVolume.h"
#include <Kernel.h>
#include <Drivers/PIT/PITDriver.h>
#include <Drivers/PS2/PS2Driver.h>

bool FatCachedSector_Constructor(struct FatCachedSector* self, struct FatVolume* parent, uint32_t sector_number)
{
    printf("FatCachedSector: Constructing\n");
    memset(self,0,sizeof(struct FatCachedSector));
    self->ParentVolume = parent;
    self->SectorNumber = sector_number;
    self->InUse = true;
    self->Invalid = false;
    self->LastAccess = self->ParentVolume->Kernel->PIT.Ticks;
}


void FatCachedSector_Debug(struct FatCachedSector* self)
{
    printf("FatCachedSector:\n");
    printf("\tSector Number: %d (0x%x)\n",self->SectorNumber,self->SectorNumber);
    printf("\tInUse:         %d\n",self->InUse);
    printf("\tInvalid:       %d\n",self->Invalid);
    printf("\tLast Access:   %d\n",self->LastAccess);
    PS2Driver_WaitForKeyPress("FatCachedSector Debug");
}