#include "MBR.h"

#include <stdio.h>
#include <string.h>
#include <Drivers/ATA/ATADriver.h>
#include <Drivers/Memory/MemoryDriver.h>
#include <Drivers/PS2/PS2Driver.h>

bool MBR_Constructor(struct MBR* self, struct ATADriver* ata, uint8_t ata_device_id)
{
    memset(self,0,sizeof(struct MBR));

    self->Debug = false;
    self->ATA = ata;
    self->IDEDevice = &self->ATA->IDEDevices[ata_device_id];

    if (self->Debug) 
    {
        printf("MBR: Constructing Record for device: %s\n",self->IDEDevice->model);
    }
	if (self->IDEDevice->reserved != 1)
	{
		printf("MBR: Error - Constructing Record Failed: No Device\n");
		return false;
	}

    uint8_t result = ATADriver_IDEAccess(self->ATA, 0,ata_device_id,0,1,(void*)&self->Record);
    if(result)
    {
        printf("MBR: IDEAccess Error %d\n",result);
        return false;
    }
    
    if (self->Debug)
    {
        MBRRecord_Debug(&self->Record);
    }
    return true;
}
