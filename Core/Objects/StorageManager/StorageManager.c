#include <Objects/StorageManager/StorageManager.h>

#include <stdio.h>
#include <string.h>
#include <Kernel.h>

bool StorageManager_Constructor(struct StorageManager* self, struct Kernel* kernel)
{
    memset(self,0,sizeof(struct StorageManager));
    self->Kernel = kernel;

    LinkedList_Constructor(&self->Drives, &kernel->Memory);

	if(kernel->ATA.IDEDevices[0].reserved)
	{
		printf("StorageManager: Initialising Drive %s\n",kernel->ATA.IDEDevices[0].model);
        struct Drive* drive = MemoryDriver_Allocate(&kernel->Memory, sizeof(struct Drive));
        //Drive_Constructor(drive, )
		
	}
	else
	{
		printf("Kernel: No Primary/Master ATA Device Found\n");
	}
}

bool StorageManager_Destructor(struct StorageManager* self)
{

}