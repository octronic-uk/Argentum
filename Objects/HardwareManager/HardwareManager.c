#include "HardwareManager.h"


#include <stdio.h>
#include <string.h>



bool HardwareManager_Constructor(HardwareManager* self)
{
    printf("HardwareManager: Constructing\n");
    memset(self,0,sizeof(HardwareManager));

	if (!StorageManager_Constructor(&self->StorageManager))
	{
		return false;
	}

	if (!GraphicsManager_Constructor(&self->GraphicsManager))
	{
		return false;
	}

    if (!InputManager_Constructor(&self->InputManager))
	{
		return false;
	}
	return true;
}

void HardwareManager_Destructor(HardwareManager* self)
{
	StorageManager_Destructor(&self->StorageManager);
	GraphicsManager_Destructor(&self->GraphicsManager);
}