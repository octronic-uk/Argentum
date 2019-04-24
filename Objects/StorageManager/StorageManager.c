#include <Objects/StorageManager/StorageManager.h>

#include <Drivers/PS2/PS2Driver.h>

#include <stdio.h>
#include <string.h>
#include <Objects/Kernel/Kernel.h>
#include "../FAT16/FatConstants.h"
#include "SM_ATADrive.h"
#include "SM_FloppyDrive.h"
#include "SMDirectoryEntry.h"

extern struct Kernel _Kernel;

bool StorageManager_Constructor(struct StorageManager* self)
{
	printf("StorageManager: Constructing\n");

    memset(self,0,sizeof(struct StorageManager));
	self->Debug = true; 

	StorageManager_ProbeFloppyDrives(self);
	StorageManager_ProbeATADrives(self);
	return true;
}

bool StorageManager_ProbeATADrives(struct StorageManager* self)
{
	uint8_t i;
	for (i=0; i<SM_MAX_ATA_DRIVES; i++)
	{
		struct ATA_IDEDevice* device = &_Kernel.ATA.IDEDevices[i];
		if(device->reserved && device->type == ATA_DEVICE_TYPE_ATA)
		{
			if (self->Debug)
			{
				printf("StorageManager: Initialising ATA Drive %d %s\n", i, device->model);
				PS2Driver_WaitForKeyPress("StorageManager Pause");
			}
			SM_ATADrive_Constructor(&self->ATADrives[i], i);
		}
	}

	return true;
}

bool StorageManager_ProbeFloppyDrives(struct StorageManager* self)
{
	return true;
}

void StorageManager_ListDrives(struct StorageManager* self)
{
	printf("StorageManager: Floppy Drives\n");
	uint8_t i;
	for (i=0;i<SM_MAX_FLOPPY_DRIVES;i++)
	{
		if (self->FloppyDrives[i].Exists)
		{
			printf("\tFound Floppy Drive %d\n",i);
		}
	}
	printf("StorageManager: ATA Drives\n");
	for (i=0;i<SM_MAX_ATA_DRIVES;i++)
	{
		if (self->ATADrives[i].Exists)
		{
			printf("\tFound ATA Drive %d\n",i);
		}
	}
}

struct SM_ATADrive* StorageManager_GetATADrive(struct StorageManager* self, uint8_t drive_id)
{
	if (self->ATADrives[drive_id].Exists)
	{
		return &self->ATADrives[drive_id];
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
	struct SM_ATADrive* drive = StorageManager_GetATADrive(self,path->DriveIndex);

	if (!drive) 
	{
		printf("SM: Error - Failed to get drive %d\n",path->DriveIndex);
		PS2Driver_WaitForKeyPress("StorageManager Pause");
		return false;
	}

	struct SMVolume* volume = SM_ATADrive_GetVolume(drive,path->VolumeIndex);
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

bool StorageManager_Test(struct StorageManager* self)
{
	printf("\n\nStorageManager: Testing StorageManager\n");
	StorageManager_ListDrives(self);
	struct SM_ATADrive* drive0 = StorageManager_GetATADrive(self, 0);

	if (drive0)
	{
		printf("StorageManager: Got Drive 0 from StorageManager\n");
		SM_ATADrive_ListVolumes(drive0);
		struct SMVolume* ata0v0 = SM_ATADrive_GetVolume(drive0,0);
		if (ata0v0)
		{
			printf("StorageManager: Got ata0p0 \n");
			PS2Driver_WaitForKeyPress("SM Pause");
		}
		else
		{
			printf("StorageManager: Error getting ata0p0");
			PS2Driver_WaitForKeyPress("SM Pause");
		}
		
	}

	SMPath_TestParser();

	char* file_path = "ata0p0://STL_FILE/SquirrelChess.rar";
	//char* file_path = "ata0p0:/test.txt";

	struct SMDirectoryEntry file;

	if(!StorageManager_Open(self, &file, file_path))
	{
		printf("StorageManager: StorageManager has FAILED to open the file at %s\n", file_path);
		PS2Driver_WaitForKeyPress("SM Pause");
		return false;
	}

	if (!SMDirectoryEntry_IsFile(&file))
	{
		printf("StorageManager: Directory %s is not a file\n", file_path);
		PS2Driver_WaitForKeyPress("SM Pause");
	}

	printf("StorageManager: StorageManager has opened the file at %s\n", file_path);
	PS2Driver_WaitForKeyPress("SM Pause");
	
	return false;
}