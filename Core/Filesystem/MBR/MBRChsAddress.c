#include <Filesystem/MBR/MBRChsAddress.h>

#include <stdio.h>
#include <string.h>

/*
    CHS0: head
    CHS1: sector in bits 5–0; bits 7–6 are high bits of cylinder
    CHS2: bits 7–0 of cylinder
*/
void MBRChsAddress_From3b(struct MBRChsAddress* self, struct MBRChsAddress3b* data)
{
    memset(&self,0,sizeof(struct MBRChsAddress));
    self->Head = data->CHS0;
    self->Sector = data->CHS1 & 0x3F;
    self->Cylinder = (data->CHS1 & 0xC0) << 2;
    self->Cylinder |= data->CHS2;
}

void MBRChsAddress_Debug(const char* name, struct MBRChsAddress addr)
{
    printf("\t\t%s:\n", name, sizeof (struct MBRChsAddress));
    printf("\t\t\tCylinder: 0x%x\n", addr.Cylinder);
    printf("\t\t\tHead: 0x%x\n", addr.Head);
    printf("\t\t\tSector: 0x%x\n", addr.Sector);
}