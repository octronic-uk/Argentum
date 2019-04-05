#include <Filesystem/MBR/MBR.h>

#include <stdio.h>
#include <string.h>
#include <Drivers/ATA/ATADriver.h>
#include <Drivers/Memory/MemoryDriver.h>

bool MBR_Constructor(struct MBR* self, struct ATADriver* ata, uint8_t ata_device_id)
{
    self->Debug = true;
    self->ATA = ata;
    self->IDEDevice = &self->ATA->IDEDevices[ata_device_id];

    printf("MBR: Constructing Record for device: %s\n",self->IDEDevice->model);
	if (self->IDEDevice->reserved != 1)
	{
		printf("MBR: Error - Constructing Record Failed: No Device\n");
		return false;
	}

    uint8_t result = ATADriver_IDEAccess(self->ATA, 0,ata_device_id,0,1,(void*)&self->Record);
    if(result)
    {
        if(self->Debug) printf("MBR: IDEAccess Error\n");
    }
    if (self->Debug)
    {
        MBRRecord_Debug(&self->Record);
    }
    return true;
}
