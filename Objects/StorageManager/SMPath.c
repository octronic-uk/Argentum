#include "SMPath.h"

#include <Objects/Kernel/Kernel.h>
#include <string.h>
#include <stdio.h>

extern Kernel _Kernel;

bool SMPath_Constructor(struct SMPath* self)
{
    memset(self,0,sizeof(struct SMPath));
    self->Debug = false;
    LinkedList_Constructor(&self->Directories);
    return true;
}

bool SMPath_ConstructAndParse(struct SMPath* self, const char* path)
{
    if (!SMPath_Constructor(self))
    {
        return false;
    }
    return SMPath_Parse(self,path);
}

bool SMPath_Destructor(struct SMPath* self)
{
    LinkedList_FreeAllData(&self->Directories);
    LinkedList_Destructor(&self->Directories);
}

bool SMPath_Parse(struct SMPath* self, const char* address)
{
    if (self->Debug) printf("SMPath: Parsing \"%s\"\n",address);

    uint32_t address_string_index = 0;
    bool drive_found = false;

    // Get device String
    uint8_t dev_idx;
    for (dev_idx = 0; dev_idx < SM_PATH_DEVICE_BUFFER_LENGTH; dev_idx++)
    {
        if (strcmp(&address[address_string_index], SM_PATH_DEVICE_DELIMETER) == 0)
        {
            drive_found = true;
            break;
        }
        self->DeviceString[dev_idx] = address[address_string_index++];
    }

    // Force null terminate the device str
    self->DeviceString[SM_PATH_DEVICE_BUFFER_LENGTH-1] = 0;
    uint8_t device_str_idx = 0;
    if (self->Debug) printf("StorageManager: Device String: %s\n",self->DeviceString);

    // is 'ATA'||'ata'
    if (strncmp(self->DeviceString,SM_PATH_BUS_ATA_STR, SM_PATH_BUS_NAME_LEN) == 0 || 
        strncmp(self->DeviceString,SM_PATH_BUS_ATA_LOWER_STR, SM_PATH_BUS_NAME_LEN) == 0)
    {
        self->DriveType = DRIVE_TYPE_ATA;
        device_str_idx = 3;
    }
    // is 'FDD'||'fdd'
    else if (strncmp(self->DeviceString, SM_PATH_BUS_FDD_STR, SM_PATH_BUS_NAME_LEN) == 0 || 
             strncmp(self->DeviceString, SM_PATH_BUS_FDD_LOWER_STR, SM_PATH_BUS_NAME_LEN) == 0)
    {
        self->DriveType = DRIVE_TYPE_FLOPPY;
        device_str_idx = 3;
    }
    // is 'RAM' || 'ram'
    else if (strncmp(self->DeviceString, SM_PATH_BUS_RAM_STR, SM_PATH_BUS_NAME_LEN) == 0 ||
             strncmp(self->DeviceString, SM_PATH_BUS_RAM_LOWER_STR, SM_PATH_BUS_NAME_LEN) == 0)
    {
        self->DriveType = DRIVE_TYPE_RAMDISK;
        device_str_idx = 3;
    }
    else
    {
        self->DriveType = DRIVE_TYPE_ERROR;
        self->ParseError = SM_PATH_ERR_INVALID_DRIVE;
        return false;
    }

    if (!drive_found)
    {
        self->ParseError = SM_PATH_ERR_INVALID_DRIVE;
        return false;
    }

    // Device Number
    if (self->DeviceString[device_str_idx])
    {
        self->DriveIndex = chrtoi(self->DeviceString[device_str_idx],10);
    }

    device_str_idx++;

    // If ATA get volume
    if (self->DriveType == DRIVE_TYPE_ATA && 
        self->DeviceString[device_str_idx] == SM_PATH_VOLUME_DELIMETER)
    {
        self->VolumeIndex = chrtoi(self->DeviceString[++device_str_idx],10);
    }

    // Consume Device Delimeter
    address_string_index+=3;


    // Allocate new name
    char* current_name = MemoryDriver_Allocate(&_Kernel.Memory, FAT_LFN_NAME_SIZE);
    memset(current_name, 0, FAT_LFN_NAME_SIZE);
    LinkedList_PushBack(&self->Directories, current_name);

    int8_t path_count_index = 0;
    int8_t name_char_index = 0;

    while (address[address_string_index])
    {
        if (address[address_string_index] == SM_PATH_DELIMETER)
        {
            if (name_char_index) 
            {
                // Allocate new name
                current_name = MemoryDriver_Allocate(&_Kernel.Memory, FAT_LFN_NAME_SIZE);
                memset(current_name, 0, FAT_LFN_NAME_SIZE);
                LinkedList_PushBack(&self->Directories, current_name);
                path_count_index++;
                name_char_index = 0;
            }
        }
        else
        {
            // Reached max size?
            if (name_char_index >= FAT_LFN_NAME_SIZE-1)
            {
                self->ParseError = SM_PATH_ERR_NAME_TOO_LONG;
                return false;
            }
            // Is valid path char?
            else if (SMPath_ValidPathChar(address[address_string_index]))
            {
                current_name[name_char_index++] = address[address_string_index];
            }
            // Invalid path char
            else
            {
                if (self->Debug)
                {
                    printf("SMPath: Error - Invalid char \"%c\"\n",address[address_string_index]);
                }
                
                self->ParseError = SM_PATH_ERR_INVALID_PATH_CHAR;
                return false;
            }
        }
        address_string_index++;
    }
    if (self->Debug)
    {
        printf("SMPath: Parsed Successfully\n");
    }
    return true;
}

/* 
    Paths support the following characters
    * A–Z 
    * a–z 
    * 0–9 
    * '.' Full Stop 
    * '_' Underscore
    * '-' Minus
    * ' ' Space
*/
bool SMPath_ValidPathChar(char c)
{
    if (c >= '0' && c <= '9') return true;
    if (c >= 'A' && c <= 'Z') return true;
    if (c >= 'a' && c <= 'z') return true;
    if (c == '.') return true;
    if (c == '_') return true;
    if (c == '-') return true;
    if (c == ' ') return true;
    return false;
}

const char* SMPath_GetParseErrorString(struct SMPath* self)
{
    switch(self->ParseError)
    {
        case SM_PATH_NO_ERROR:
            return "No Error";
        case SM_PATH_ERR_INVALID_DRIVE:
            return "Invalid drive id";
        case SM_PATH_ERR_INVALID_VOLUME:
            return "Invalid volume id";
        case SM_PATH_ERR_INVALID_PATH_CHAR:
            return "Invalid character in path";
        case SM_PATH_ERR_BLANK_NAME:
            return "Blank name in path (multiple /'s together)";
        case SM_PATH_ERR_NAME_TOO_LONG:
            return "Directory or file name too long";
        case SM_PATH_ERR_NO_ROOT_DELIMETER:
            return "Path is missing root delimeter";

        default:
            "Unknown Error";
    }
}

void SMPath_Debug(struct SMPath* self)
{

    char* drive_type = "ERROR";

    if (self->DriveType == DRIVE_TYPE_ATA)
    {
        drive_type = "ATA";
    }
    else if (self->DriveType == DRIVE_TYPE_FLOPPY)
    {
        drive_type = "Floppy";
    }
    else if (self->DriveType == DRIVE_TYPE_RAMDISK)
    {
        drive_type = "RamDisk";
    }

    printf("SMPath:\n");
    printf("\tType:   %s\n", drive_type);
    printf("\tDevice: %s\n", self->DeviceString);
    printf("\tDrive:  %d\n", self->DriveIndex);
    if (self->DriveType == DRIVE_TYPE_ATA)
    {
       printf("\tVolume: %d\n", self->VolumeIndex);
    }
    
    uint8_t i, count;
    count = LinkedList_Size(&self->Directories);
    for (i=0; i<count; i++)
    {
        char* dir = (char*)LinkedList_At(&self->Directories,i);
        printf("\tDirectory %d : %s\n",i,dir);
    }
}


void SMPath_TestParser()
{
    printf("\n\nStorageManager: Testing Parser...\n");
    struct SMPath addr1, addr2, addr3, addr4;

	char* test_address_1 = "ata1p0://dir_1";
	char* test_address_2 = "FDD1://dir_1/some_file.ext";
	char* test_address_3 = "fdd0:///dir_1/dir_2/file.ext";
	char* test_address_4 = "ram0:///dir_x/dir_y/file_z.ext";

	SMPath_Constructor(&addr1);
	SMPath_Constructor(&addr2);
	SMPath_Constructor(&addr3);
    SMPath_Constructor(&addr4);

    bool retval = 1;

	if (SMPath_Parse(&addr1, test_address_1)) SMPath_Debug(&addr1);
	else
	{
        printf("SMPath: Failed to parse address 1: %s\n",SMPath_GetParseErrorString(&addr1));
        retval = 0;
	}

	if (retval && SMPath_Parse(&addr2, test_address_2)) SMPath_Debug(&addr2);
	else
	{
		printf("SMPath: Failed to parse address 2: %s\n",SMPath_GetParseErrorString(&addr2));
        retval = 0;
	}

	if (retval && SMPath_Parse(&addr3, test_address_3)) SMPath_Debug(&addr3);
	else
	{
		printf("SMPath: Failed to parse address 3: %s\n",SMPath_GetParseErrorString(&addr3));
        retval = 0;
	}

    if (retval && SMPath_Parse(&addr4, test_address_4)) SMPath_Debug(&addr4);
	else
	{
		printf("SMPath: Failed to parse address 4: %s\n",SMPath_GetParseErrorString(&addr4));
        retval = 0;
	}

    SMPath_Destructor(&addr1);
    SMPath_Destructor(&addr2);
    SMPath_Destructor(&addr3);
    SMPath_Destructor(&addr4);

    return;
}