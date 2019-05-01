#include "StorageManager.h"

#include <stdio.h>
#include <string.h>

#include <Drivers/PS2/PS2Driver.h>
#include <Drivers/Floppy/FloppyDriver.h>
#include <Objects/Kernel/Kernel.h>
#include <Objects/FAT16/FatConstants.h>
#include "SMDrive.h"
#include "SMDirectoryEntry.h"

extern struct Kernel _Kernel;

bool StorageManager_Constructor(struct StorageManager* self)
{
	printf("StorageManager: Constructing\n");

    memset(self,0,sizeof(struct StorageManager));
	self->Debug = false; 

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
			SMDrive_ATAConstructor(&self->AtaDrives[i], i);
		}
	}

	return true;
}

bool StorageManager_ProbeFloppyDrives(struct StorageManager* self)
{
	if (FloppyDriver_MasterPresent(&_Kernel.Floppy))
	{
		printf("StorageManager: Initialising Floppy 0 (Master)\n");
		SMDrive_FloppyConstructor(&self->FloppyDrives[0], 0);
	}
	if (FloppyDriver_SlavePresent(&_Kernel.Floppy))
	{
		printf("StorageManager: Initialising Floppy 1 (Slave)\n");
		SMDrive_FloppyConstructor(&self->FloppyDrives[1], 1);
	}
	return true;
}

void StorageManager_ListDrives(struct StorageManager* self)
{
	printf("StorageManager: Drives\n");
	if (FloppyDriver_MasterPresent(&_Kernel.Floppy))
	{
		printf("\tFound Floppy 0 (Master)\n");
	}
	if (FloppyDriver_SlavePresent(&_Kernel.Floppy))
	{
		printf("\tFound Floppy 1 (Slave)\n");
	}

	uint32_t i;
	for (i=0;i<SM_MAX_ATA_DRIVES;i++)
	{
		if (self->AtaDrives[i].Exists)
		{
			printf("\tFound ATA Drive %d\n",i);
		}
	}
}

struct SMDrive* StorageManager_GetATADrive(struct StorageManager* self, uint8_t drive_id)
{
	if (self->AtaDrives[drive_id].Exists)
	{
		return &self->AtaDrives[drive_id];
	}
	else
	{
		return 0;
	}
}

struct SMDrive* StorageManager_GetFloppyDrive(struct StorageManager* self, uint8_t drive_id)
{
	if (self->FloppyDrives[drive_id].Exists)
	{
		return &self->FloppyDrives[drive_id];
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
	struct SMDrive* drive = 0;
	if (path->DriveType == DRIVE_TYPE_ATA)
	{
		if (self->Debug) printf("StorageManager: Opening file on ATA Drive\n");
		drive = StorageManager_GetATADrive(self,path->DriveIndex);
	}
	else if (path->DriveType == DRIVE_TYPE_FLOPPY)
	{
		if (self->Debug) printf("StorageManager: Opening file on Floppy Drive\n");
		drive = StorageManager_GetFloppyDrive(self,path->DriveIndex);
	}
	else
	{
		printf("StorageManager: Error- Cannot open directory, path has invalid drive type\n");
		PS2Driver_WaitForKeyPress("StorageManager Pause");
		return false;
	}

	if (!drive) 
	{
		printf("StorageManager: Error - Failed to get drive\n");
		PS2Driver_WaitForKeyPress("StorageManager Pause");
		return false;
	}

	struct SMVolume* volume = SMDrive_GetVolume(drive,path->VolumeIndex);
	if (!volume) 
	{
		printf("StorageManager: Failed to get volume %d\n",path->VolumeIndex);
		PS2Driver_WaitForKeyPress("StorageManager Pause");
		return false;
	}

	// Read root directory
	uint8_t root_sector_buffer[FAT_SECTOR_SIZE];
	memset(root_sector_buffer,0,FAT_SECTOR_SIZE);

	if (self->Debug)
	{
		printf("StorageManager: Reading root sector\n");
		PS2Driver_WaitForKeyPress("StorageManager Pause");
	}

	if(FatVolume_ReadSector(&volume->FatVolume, volume->FatVolume.RootDirSectorNumber, root_sector_buffer))
	{
		if (self->Debug)
		{
			FatVolume_DebugSector(root_sector_buffer);
			printf("StorageManager: Starting GetDirectory Recursion\n");
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
	struct SMDrive* drive0 = StorageManager_GetATADrive(self, 0);

	if (drive0)
	{
		printf("StorageManager: Got Drive 0 from StorageManager\n");
		SMDrive_ATAListVolumes(drive0);
		struct SMVolume* ata0v0 = SMDrive_GetVolume(drive0,0);
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

	// Floppy
	char* fdd_file_path = "fdd0://HELP.COM";
	struct SMDirectoryEntry fdd_file;

	if(StorageManager_Open(self, &fdd_file, fdd_file_path))
	{
		if (SMDirectoryEntry_IsFile(&fdd_file))
		{
			printf("StorageManager: StorageManager has opened the file at %s\n", fdd_file_path);
			PS2Driver_WaitForKeyPress("SM Pause");
		}
		else
		{
			printf("StorageManager: Directory %s is not a file\n", fdd_file_path);
			PS2Driver_WaitForKeyPress("SM Pause");
		}
	}
	else 
	{
		printf("StorageManager: StorageManager has FAILED to open the file at %s\n", fdd_file_path);
		PS2Driver_WaitForKeyPress("SM Pause");
	}

	// ATA
	char* ata_file_path = "ata0p0://STL_FILE/SquirrelChess.rar";
	struct SMDirectoryEntry ata_file;

	if(StorageManager_Open(self, &ata_file, ata_file_path))
	{
		if (SMDirectoryEntry_IsFile(&ata_file))
		{
			printf("StorageManager: StorageManager has opened the file at %s\n", ata_file_path);
			PS2Driver_WaitForKeyPress("SM Pause");
		}
		else
		{
			printf("StorageManager: Directory %s is not a file\n", ata_file_path);
			PS2Driver_WaitForKeyPress("SM Pause");
		}
	}
	else
	{
		printf("StorageManager: StorageManager has FAILED to open the file at %s\n", ata_file_path);
		PS2Driver_WaitForKeyPress("SM Pause");
	}
	
	return true;
}