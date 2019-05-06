#include "StorageManager.h"

#include <stdio.h>
#include <string.h>

#include <Drivers/Floppy/FloppyDriver.h>
#include <Objects/Kernel/Kernel.h>
#include <Objects/FAT16/FatConstants.h>
#include <Objects/RamDisk/RamDisk.h>
#include <Objects/StorageManager/SMDrive.h>
#include <Objects/StorageManager/SMDirectoryEntry.h>

extern struct Kernel _Kernel;

bool StorageManager_Constructor(struct StorageManager* self)
{
	printf("StorageManager: Constructing\n");

    memset(self,0,sizeof(struct StorageManager));
	self->Debug = false; 
	LinkedList_Constructor(&self->RamDisks);
	LinkedList_Constructor(&self->Drives);

	StorageManager_SetupRamDisk0(self);
	StorageManager_ProbeFloppyDrives(self);
	StorageManager_ProbeATADrives(self);
	return true;
}

void StorageManager_Destructor(struct StorageManager *self)
{
	printf("StorageManager: Destructing\n");

	// Drives
	uint32_t i, num_drives = LinkedList_Size(&self->Drives);
	for (i=0; i<num_drives; i++)
	{
		struct SMDrive* drive = (struct SMDrive*)LinkedList_At(&self->Drives, i);
		SMDrive_Destructor(drive);
	}
	LinkedList_FreeAllData(&self->Drives);
	LinkedList_Destructor(&self->Drives);

	// Ram Disks
	uint32_t num_ram_disks = LinkedList_Size(&self->RamDisks);
	for (i=0; i<num_ram_disks; i++)
	{
		struct RamDisk* rd = (struct RamDisk*)LinkedList_At(&self->RamDisks, i);
		RamDisk_Destructor(rd);
	}
	LinkedList_FreeAllData(&self->RamDisks);
	LinkedList_Destructor(&self->RamDisks);
}

void StorageManager_SetupRamDisk0(struct StorageManager* self)
{
	struct RamDisk* rd = (struct RamDisk*)MemoryDriver_Allocate(&_Kernel.Memory,sizeof (struct RamDisk));
	LinkedList_PushBack(&self->RamDisks, rd);
	RamDisk_Constructor(rd, RAMDISK_SIZE_1MB*10);

	struct SMDrive* drive = (struct SMDrive*)MemoryDriver_Allocate(&_Kernel.Memory, sizeof(struct SMDrive));
	LinkedList_PushBack(&self->Drives, drive);
	SMDrive_RamDiskConstructor(drive,0);
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
			}
			struct SMDrive* drive = (struct SMDrive*)MemoryDriver_Allocate(&_Kernel.Memory, sizeof(struct SMDrive));
			LinkedList_PushBack(&self->Drives,drive);
			SMDrive_ATAConstructor(drive, i);
		}
	}

	return true;
}

bool StorageManager_ProbeFloppyDrives(struct StorageManager* self)
{
	if (FloppyDriver_MasterPresent(&_Kernel.Floppy))
	{
		printf("StorageManager: Initialising Floppy 0 (Master)\n");
		struct SMDrive* drive = (struct SMDrive*)MemoryDriver_Allocate(&_Kernel.Memory, sizeof(struct SMDrive));
		LinkedList_PushBack(&self->Drives, drive);
		SMDrive_FloppyConstructor(drive, 0);
	}
	if (FloppyDriver_SlavePresent(&_Kernel.Floppy))
	{
		printf("StorageManager: Initialising Floppy 1 (Slave)\n");
		struct SMDrive* drive = (struct SMDrive*)MemoryDriver_Allocate(&_Kernel.Memory, sizeof(struct SMDrive));
		LinkedList_PushBack(&self->Drives, drive);
		SMDrive_FloppyConstructor(drive, 1);
	}
	return true;
}

void StorageManager_ListDrives(struct StorageManager* self)
{
	printf("StorageManager: Drives\n");

	uint32_t i, count = LinkedList_Size(&self->Drives);

	for (i=0; i<count; i++)
	{
		struct SMDrive* drive = (struct SMDrive*)LinkedList_At(&self->Drives,i);
		if (drive->Exists)
		{
			enum SMDriveType type = SMDrive_GetDriveType(drive);
			char* type_str;
			int8_t index = -1; 
			switch (type)
			{
				case SM_DRIVE_TYPE_ATA: 
					type_str = "ata";
					index = drive->AtaIndex;
					break;
				case SM_DRIVE_TYPE_FLOPPY: 
					type_str = "fdd";
					index = drive->FloppyIndex;
					break;
				case SM_DRIVE_TYPE_RAMDISK: 
					type_str = "ram";
					index = drive->RamDiskIndex;
					break;
				default:
					type_str = "none";
					break;
			}
			printf("\tFound SMDrive %s%d\n",type_str, index);
		}
	}
}

struct SMDrive* StorageManager_GetATADrive(struct StorageManager* self, uint8_t drive_id)
{
	uint32_t i, count = LinkedList_Size(&self->Drives);
	for (i=0; i<count; i++)
	{
		struct SMDrive* drive = (struct SMDrive*)LinkedList_At(&self->Drives,i);
		if (drive->AtaIndex == drive_id && drive->Exists)
		{
			return drive;
		}
	}
	return 0;
}

struct SMDrive* StorageManager_GetFloppyDrive(struct StorageManager* self, uint8_t drive_id)
{
	uint32_t i, count = LinkedList_Size(&self->Drives);
	for (i=0; i<count; i++)
	{
		struct SMDrive* drive = (struct SMDrive*)LinkedList_At(&self->Drives,i);
		if (drive->FloppyIndex == drive_id && drive->Exists)
		{
			return drive;
		}
	}
	return 0;
}

struct SMDrive* StorageManager_GetRamDiskDrive(struct StorageManager* self, uint8_t drive_id)
{
	uint32_t i, count = LinkedList_Size(&self->Drives);
	for (i=0; i<count; i++)
	{
		struct SMDrive* drive = (struct SMDrive*)LinkedList_At(&self->Drives,i);
		if (drive->RamDiskIndex == drive_id && drive->Exists)
		{
			return drive;
		}
	}
	return 0;
}

bool StorageManager_Open(struct StorageManager* self, struct SMDirectoryEntry* dir, const char* path_str)
{
	if (self->Debug) 
	{
		printf("StorageManager: Opening file %s\n",path_str);
	}
	struct SMPath path;
	if (SMPath_ConstructAndParse(&path,path_str))
	{
		bool retval = StorageManager_OpenPath(self,dir,&path);
		SMPath_Destructor(&path);
		return retval;
	}
	if (self->Debug) 
	{
		printf("StorageManager: Failed to Construct/Parse Path: %s\n", SMPath_GetParseErrorString(&path));
	}
	return false;
}

bool StorageManager_OpenPath(struct StorageManager* self, struct SMDirectoryEntry* dir, struct SMPath* path)
{
	printf("StorageManager: Opening Path\n");
	SMPath_Debug(path);

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
	else if (path->DriveType == DRIVE_TYPE_RAMDISK)
	{
		if (self->Debug) printf("StorageManager: Opening file on RamDisk Drive\n");
		drive = StorageManager_GetRamDiskDrive(self,path->DriveIndex);
	}
	else
	{
		printf("StorageManager: Error- Cannot open directory, path has invalid drive type\n");
		return false;
	}

	if (!drive) 
	{
		printf("StorageManager: Error - Failed to get drive\n");
		return false;
	}

	struct SMVolume* volume = SMDrive_GetVolume(drive,path->VolumeIndex);
	if (!volume) 
	{
		printf("StorageManager: Failed to get volume %d\n",path->VolumeIndex);
		return false;
	}

	uint32_t sector_count = SMVolume_GetRootDirectorySectorCount(volume);
	uint32_t root_buffer_size = sector_count*FAT_SECTOR_SIZE;

	// Read root directory
	// TODO leaky leaky
	uint8_t* root_sector_buffer = MemoryDriver_Allocate(&_Kernel.Memory, root_buffer_size);
	memset(root_sector_buffer, 0, root_buffer_size);

	if (self->Debug)
	{
		printf("StorageManager: Reading root directory sector(s)\n");
	}

	uint32_t i;
	for (i=0; i<sector_count; i++)
	{
		if(!FatVolume_ReadSector(&volume->FatVolume, volume->FatVolume.RootDirSectorNumber+i, &root_sector_buffer[FAT_SECTOR_SIZE*i]))
		{
			printf("StorageManager: Error unable to read sector %d of volume\n",i);
			MemoryDriver_Free(&_Kernel.Memory,root_sector_buffer);
			return false;
		}
	}

	if (self->Debug)
	{
		FatVolume_DebugSector(root_sector_buffer);
		printf("StorageManager: Starting GetDirectory Recursion\n");
	}
	bool retval =  SMVolume_GetDirectory(volume, dir, root_sector_buffer, sector_count, path);
	MemoryDriver_Free(&_Kernel.Memory,root_sector_buffer);
	return retval;
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
		}
		else
		{
			printf("StorageManager: Error getting ata0p0");
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
		}
		else
		{
			printf("StorageManager: Directory %s is not a file\n", fdd_file_path);
		}
	}
	else 
	{
		printf("StorageManager: StorageManager has FAILED to open the file at %s\n", fdd_file_path);
	}

	// ATA
	char* ata_file_path = "ata0p0://STL_FILE/SquirrelChess.rar";
	struct SMDirectoryEntry ata_file;

	if(StorageManager_Open(self, &ata_file, ata_file_path))
	{
		if (SMDirectoryEntry_IsFile(&ata_file))
		{
			printf("StorageManager: StorageManager has opened the file at %s\n", ata_file_path);
		}
		else
		{
			printf("StorageManager: Directory %s is not a file\n", ata_file_path);
		}
	}
	else
	{
		printf("StorageManager: StorageManager has FAILED to open the file at %s\n", ata_file_path);
	}
	
	return true;
}