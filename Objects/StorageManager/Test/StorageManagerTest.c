#include "StorageManagerTest.h"

#include <stdio.h>
#include <stdlib.h>
#include <Objects/StorageManager/StorageManager.h>
#include <Objects/StorageManager/SMDirectoryEntry.h>

bool StorageManagerTest_RunSuite(StorageManager* self)
{
	printf("\n\nStorageManager: Testing StorageManager\n");
	StorageManager_ListDrives(self);
	SMDrive* drive0 = StorageManager_GetATADrive(self, 0);

	if (drive0)
	{
		printf("StorageManager: Got Drive 0 from StorageManager\n");
		SMDrive_ATAListVolumes(drive0);
		SMVolume* ata0v0 = SMDrive_GetVolume(drive0,0);
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
	char* fdd_file_path = "fdd0://HELP.HL_";
	SMDirectoryEntry fdd_file;

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
	char* ata_file_path = "ata0p0://Torak.bmp";
	//char* ata_file_path = "ata0p0://test.txt";
	SMDirectoryEntry ata_file;

	if(StorageManager_Open(self, &ata_file, ata_file_path))
	{
		if (SMDirectoryEntry_IsFile(&ata_file))
		{
			printf("StorageManager: StorageManager has opened the file at %s\n", ata_file_path);

			uint32_t test_buf_sz = ata_file.FatDirectoryEntrySummary.FileSize;
			uint32_t test_offset = 0;
			SMDirectoryEntry_SetFileOffset(&ata_file, test_offset);
			uint8_t* buf = (uint8_t*)malloc(test_buf_sz);
			SMDirectoryEntry_Read(&ata_file, test_buf_sz, buf);
			StorageManagerTest_DebugData(buf,test_buf_sz);
			free(buf);
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

void StorageManagerTest_DebugData(uint8_t* data, uint32_t size)
{
    int i;
    for (i=0; i<size; i++)
    {
		if (i % 512 == 0)
		{
			printf("\n\n        00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F\n");
			    printf("        -----------------------------------------------------------------------------------------------");
		}
        if (i % 32 == 0)
        {
            printf("\n0x%04x: ",i);
        }
        printf("%02x ",data[i]);
    }
	printf("\n");
}