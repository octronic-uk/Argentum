#include "StorageManager.h"

#include <stdio.h>
#include <string.h>

#include <Drivers/Floppy/FloppyDriver.h>
#include <Objects/Kernel/Kernel.h>
#include "FAT/FatConstants.h"
#include "RamDisk/RamDisk.h"
#include "SMDrive.h"
#include "SMDirectoryEntry.h"
#include <Clib/stdio/FILE.h>

extern Kernel _Kernel;

bool StorageManager_Constructor(StorageManager* self)
{
	printf("StorageManager: Constructing\n");

    memset(self,0,sizeof(StorageManager));
	self->Debug = false; 
	LinkedList_Constructor(&self->RamDisks);
	LinkedList_Constructor(&self->Drives);
	LinkedList_Constructor(&self->OpenFiles);

	StorageManager_SetupRamDisk0(self);
	StorageManager_ProbeFloppyDrives(self);
	StorageManager_ProbeATADrives(self);
	return true;
}

void StorageManager_Destructor(StorageManager *self)
{
	printf("StorageManager: Destructing\n");

	// Drives
	uint32_t i, num_drives = LinkedList_Size(&self->Drives);
	for (i=0; i<num_drives; i++)
	{
		SMDrive* drive = (SMDrive*)LinkedList_At(&self->Drives, i);
		SMDrive_Destructor(drive);
	}
	LinkedList_FreeAllData(&self->Drives);
	LinkedList_Destructor(&self->Drives);

	// Ram Disks
	uint32_t num_ram_disks = LinkedList_Size(&self->RamDisks);
	for (i=0; i<num_ram_disks; i++)
	{
		RamDisk* rd = (RamDisk*)LinkedList_At(&self->RamDisks, i);
		RamDisk_Destructor(rd);
	}
	LinkedList_FreeAllData(&self->RamDisks);
	LinkedList_Destructor(&self->RamDisks);

	LinkedList_FreeAllData(&self->OpenFiles);
	LinkedList_Destructor(&self->OpenFiles);
}

void StorageManager_SetupRamDisk0(StorageManager* self)
{
	RamDisk* rd = (RamDisk*)MemoryDriver_Allocate(&_Kernel.Memory,sizeof (RamDisk));
	LinkedList_PushBack(&self->RamDisks, rd);
	RamDisk_Constructor(rd, RAMDISK_SIZE_1MB*10);

	SMDrive* drive = (SMDrive*)MemoryDriver_Allocate(&_Kernel.Memory, sizeof(SMDrive));
	LinkedList_PushBack(&self->Drives, drive);
	SMDrive_RamDiskConstructor(drive,0);
}

bool StorageManager_ProbeATADrives(StorageManager* self)
{
	uint8_t i;
	for (i=0; i<SM_MAX_ATA_DRIVES; i++)
	{
		ATA_IDEDevice* device = &_Kernel.ATA.IDEDevices[i];
		if(device->reserved && device->type == ATA_DEVICE_TYPE_ATA)
		{
			if (self->Debug)
			{
				printf("StorageManager: Initialising ATA Drive %d %s\n", i, device->model);
			}
			SMDrive* drive = (SMDrive*)MemoryDriver_Allocate(&_Kernel.Memory, sizeof(SMDrive));
			LinkedList_PushBack(&self->Drives,drive);
			SMDrive_ATAConstructor(drive, i);
		}
	}

	return true;
}

bool StorageManager_ProbeFloppyDrives(StorageManager* self)
{
	if (FloppyDriver_MasterPresent(&_Kernel.Floppy))
	{
		printf("StorageManager: Initialising Floppy 0 (Master)\n");
		SMDrive* drive = (SMDrive*)MemoryDriver_Allocate(&_Kernel.Memory, sizeof(SMDrive));
		LinkedList_PushBack(&self->Drives, drive);
		SMDrive_FloppyConstructor(drive, 0);
	}
	if (FloppyDriver_SlavePresent(&_Kernel.Floppy))
	{
		printf("StorageManager: Initialising Floppy 1 (Slave)\n");
		SMDrive* drive = (SMDrive*)MemoryDriver_Allocate(&_Kernel.Memory, sizeof(SMDrive));
		LinkedList_PushBack(&self->Drives, drive);
		SMDrive_FloppyConstructor(drive, 1);
	}
	return true;
}

void StorageManager_ListDrives(StorageManager* self)
{
	printf("StorageManager: Drives\n");

	uint32_t i, count = LinkedList_Size(&self->Drives);

	for (i=0; i<count; i++)
	{
		SMDrive* drive = (SMDrive*)LinkedList_At(&self->Drives,i);
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

SMDrive* StorageManager_GetATADrive(StorageManager* self, uint8_t drive_id)
{
	uint32_t i, count = LinkedList_Size(&self->Drives);
	for (i=0; i<count; i++)
	{
		SMDrive* drive = (SMDrive*)LinkedList_At(&self->Drives,i);
		if (drive->AtaIndex == drive_id && drive->Exists)
		{
			return drive;
		}
	}
	return 0;
}

SMDrive* StorageManager_GetFloppyDrive(StorageManager* self, uint8_t drive_id)
{
	uint32_t i, count = LinkedList_Size(&self->Drives);
	for (i=0; i<count; i++)
	{
		SMDrive* drive = (SMDrive*)LinkedList_At(&self->Drives,i);
		if (drive->FloppyIndex == drive_id && drive->Exists)
		{
			return drive;
		}
	}
	return 0;
}

SMDrive* StorageManager_GetRamDiskDrive(StorageManager* self, uint8_t drive_id)
{
	uint32_t i, count = LinkedList_Size(&self->Drives);
	for (i=0; i<count; i++)
	{
		SMDrive* drive = (SMDrive*)LinkedList_At(&self->Drives,i);
		if (drive->RamDiskIndex == drive_id && drive->Exists)
		{
			return drive;
		}
	}
	return 0;
}

bool StorageManager_Open(StorageManager* self, SMDirectoryEntry* dir, const char* path_str)
{
	if (self->Debug) 
	{
		printf("StorageManager: Opening file %s\n",path_str);
	}
	SMPath path;
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

bool StorageManager_OpenPath(StorageManager* self, SMDirectoryEntry* dir, SMPath* path)
{
	printf("StorageManager: Opening Path\n");
	SMPath_Debug(path);

	SMDrive* drive = 0;
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

	SMVolume* volume = SMDrive_GetVolume(drive,path->VolumeIndex);
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

FILE* StorageManager_RequestFilePointer(StorageManager* self)
{
	if (self->Debug) printf("StorageManager: Requesting File Pointer\nn");
	FILE* f = MemoryDriver_Allocate(&_Kernel.Memory, sizeof(FILE));
	LinkedList_PushBack(&self->OpenFiles, f);
	return f;
}

void StorageManager_CloseFilePointer(StorageManager* self, FILE* f)
{
	LinkedList_Delete(&self->OpenFiles, f);
	MemoryDriver_Free(&_Kernel.Memory, f);
}