#include "MBRChsAddress.h"

#include <stdio.h>
#include <string.h>

/*
    CHS0: head
    CHS1: sector in bits 5–0; bits 7–6 are high bits of cylinder
    CHS2: bits 7–0 of cylinder
*/
void MBRChsAddress_From3b(MBRChsAddress* self, MBRChsAddress3b* data)
{
    memset(&self,0,sizeof(MBRChsAddress));
    self->Head = data->CHS0;
    self->Sector = data->CHS1 & 0x3F;
    self->Cylinder = (data->CHS1 & 0xC0) << 2;
    self->Cylinder |= data->CHS2;
}

void MBRChsAddress_Debug(const char* name, MBRChsAddress addr)
{
    printf("\t\t%s:\n", name, sizeof (MBRChsAddress));
    printf("\t\t\tCylinder: 0x%x\n", addr.Cylinder);
    printf("\t\t\tHead: 0x%x\n", addr.Head);
    printf("\t\t\tSector: 0x%x\n", addr.Sector);
}