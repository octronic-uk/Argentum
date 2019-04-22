#include <Objects/StorageManager/StorageManager.h>

#include <Drivers/PS2/PS2Driver.h>

#include <stdio.h>
#include <string.h>
#include <Kernel.h>
#include "FAT16/FatConstants.h"
#include "SMDrive.h"

bool StorageManager_Constructor(struct StorageManager* self, struct Kernel* kernel)
{
	printf("StorageManager: Constructing\n");

    memset(self,0,sizeof(struct StorageManager));
	self->Debug = true; 
    self->Kernel = kernel;

	uint8_t i;
	for (i=0; i<SM_MAX_DRIVES; i++)
	{
		struct ATA_IDEDevice* device = &self->Kernel->ATA.IDEDevices[i];
		if(device->reserved && device->type == ATA_DEVICE_TYPE_ATA)
		{
			if (self->Debug)
			{
				printf("StorageManager: Initialising ATA Drive %d %s\n", i, device->model);
				PS2Driver_WaitForKeyPress("StorageManager Pause");
			}
			SMDrive_Constructor(&self->Drives[i], self->Kernel, i);
			self->DriveExists[i] = true;
		}
	}

	return true;
}

void StorageManager_ListDrives(struct StorageManager* self)
{
	printf("StorageManager: Listing Drives\n");
	uint8_t i;
	for (i=0;i<SM_MAX_DRIVES;i++)
	{
		if (self->DriveExists[i])
		{
			printf("drive%d\n",i);
		}
	}
}

struct SMDrive* StorageManager_GetDrive(struct StorageManager* self, uint8_t drive_id)
{
	if (self->DriveExists[drive_id])
	{
		return &self->Drives[drive_id];
	}
	else
	{
		return 0;
	}
	
}

bool StorageManager_Open(struct StorageManager* self, struct SMDirectoryEntry* dir, const char* path_str)
{
	if (self->Debug) 
	{
		printf("StorageManager: Opening file %s\n",path_str);
		PS2Driver_WaitForKeyPress("StorageManager Pause");
	}
	struct SMPath path;
	if (SMPath_ConstructAndParse(&path,path_str))
	{
		return StorageManager_OpenPath(self,dir,&path);
	}
	if (self->Debug) 
	{
		printf("StorageManager: Failed to Construct/Parse Path: %s\n", SMPath_GetParseErrorString(&path));
		PS2Driver_WaitForKeyPress("StorageManager Pause");
	}
	return false;
}

bool StorageManager_OpenPath(struct StorageManager* self, struct SMDirectoryEntry* dir, struct SMPath* path)
{
	struct SMDrive* drive = StorageManager_GetDrive(self,path->DriveIndex);
	if (!drive) 
	{
		printf("SM: Error - Failed to get drive %d\n",path->DriveIndex);
		PS2Driver_WaitForKeyPress("StorageManager Pause");
		return false;
	}

	struct SMVolume* volume = SMDrive_GetVolume(drive,path->VolumeIndex);
	if (!volume) 
	{
		printf("SM: Failed to get volume %d\n",path->VolumeIndex);
		PS2Driver_WaitForKeyPress("StorageManager Pause");
		return false;
	}

	// Read root directory
	uint8_t root_sector_buffer[FAT_SECTOR_SIZE];
	memset(root_sector_buffer,0,FAT_SECTOR_SIZE);

	if (self->Debug)
	{
		printf("SM: Reading root sector\n");
		PS2Driver_WaitForKeyPress("StorageManager Pause");
	}
	if(FatVolume_ReadSector(&volume->FatVolume,volume->FatVolume.RootDirSectorNumber,root_sector_buffer))
	{
		if (self->Debug)
		{
			FatVolume_DebugSector(root_sector_buffer);
			printf("SM: Starting GetDirectory Recursion\n");
			PS2Driver_WaitForKeyPress("StorageManager Pause");
		}
		return SMVolume_GetDirectory(volume, dir, root_sector_buffer, path);
	}
	return false;
}