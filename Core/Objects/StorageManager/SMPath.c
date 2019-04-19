#include "SMPath.h"

#include <string.h>
#include <stdio.h>
#include <Drivers/PS2/PS2Driver.h>

bool SMPath_Constructor(struct SMPath* self)
{
    memset(self,0,sizeof(struct SMPath));
    self->Debug = false;
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

bool SMPath_Parse(struct SMPath* self, const char* address)
{
    if (self->Debug)
    {
        printf("SMPath: Parsing \"%s\"\n",address);
    }
    uint32_t addr_index = 0;

    // Bus/Drive,Volume opening Char
    if (address[addr_index++] != SM_PATH_DELIMETER)
    {
        self->ParseError = SM_PATH_ERR_NO_BEGIN_CHAR;
        return false;
    }

    // Get Bus String
    uint8_t bus_idx;
    for (bus_idx = 0; bus_idx < SM_PATH_BUS_BUFFER_LENGTH; bus_idx++)
    {
        if (address[addr_index] == SM_PATH_DELIMETER)
        {
            addr_index++;
            break;
        }
        self->Bus[bus_idx] = address[addr_index];
    }

    // Drive Number
    self->DriveIndex = char_to_int(address[addr_index++],10);
    if (self->DriveIndex > SM_PATH_MAX_DRIVE)
    {
        self->ParseError = SM_PATH_ERR_INVALID_DRIVE;
        return false;
    }

    // Drive,Volume Separator Char
    if (address[addr_index++] != SM_PATH_DELIMETER)
    {
        self->ParseError = SM_PATH_ERR_INVALID_VOLUME;
        return false;
    }

    // Volume Number
    self->VolumeIndex = char_to_int(address[addr_index++],10);
    if (self->VolumeIndex > SM_PATH_MAX_VOLUME)
    {
        self->ParseError = SM_PATH_ERR_INVALID_VOLUME;
        return false;
    }

    int8_t path_index = 0;
    int8_t name_index = 0;

    // Root Delimeter
    if (address[addr_index++] != SM_PATH_DELIMETER)
    {
        self->ParseError = SM_PATH_ERR_NO_ROOT_DELIMETER;
        return false;
    }

    while (address[addr_index])
    {
        if (address[addr_index] == SM_PATH_DELIMETER)
        {
            if (name_index) 
            {
                path_index++;
                name_index = 0;
            }
        }
        else
        {
            // Reached max size?
            if (name_index >= FAT_LFN_NAME_SIZE-1)
            {
                self->ParseError = SM_PATH_ERR_NAME_TOO_LONG;
                return false;
            }
            // Is valid path char?
            else if (SMPath_ValidPathChar(address[addr_index]))
            {
                self->Directories[path_index][name_index++] = address[addr_index];
            }
            // Invalid path char
            else
            {
                if (self->Debug)
                {
                    printf("SMPath: Error - Invalid char \"%c\"\n",address[addr_index]);
                    PS2Driver_WaitForKeyPress("SMPath Pause");
                }
                
                self->ParseError = SM_PATH_ERR_INVALID_PATH_CHAR;
                return false;
            }
        }
        addr_index++;
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
        case SM_PATH_ERR_NO_BEGIN_CHAR:
            return "No starting '/' before drive id";
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
        case SM_PATH_ERR_INVALID_BUS:
            return "Bus is invalid";

        default:
            "Unknown Error";
    }
}

void SMPath_Debug(struct SMPath* self)
{
    printf("SMPath:\n");
    printf("\tDrive:  %d\n",self->DriveIndex);
    printf("\tVolume: %d\n",self->VolumeIndex);
    uint8_t i;
    for (i=0; i<SM_PATH_MAX_DIRS; i++)
    {
        if (!self->Directories[i][0]) break;

        printf("\tDirectory %d : %s\n",i,self->Directories[i]);
    }
}


void SMPath_TestParser()
{
    struct SMPath addr1, addr2, addr3;

	char* test_address_1 = "/ATA/1/2/dir_1";
	char* test_address_2 = "/FD/1/2/dir_1/some_file.ext";
	char* test_address_3 = "/FD/0/0/dir_1/dir_2/file.ext";

	SMPath_Constructor(&addr1);
	SMPath_Constructor(&addr2);
	SMPath_Constructor(&addr3);

	if (SMPath_Parse(&addr1, test_address_1))
	{
		SMPath_Debug(&addr1);
	}
	else
	{
        printf("SMPath: Failed to parse address 1: %s\n",SMPath_GetParseErrorString(&addr1));
        PS2Driver_WaitForKeyPress("SMPath Pause");
        return;
	}

	if (SMPath_Parse(&addr2, test_address_2))
	{
		SMPath_Debug(&addr2);
	}
	else
	{
		printf("SMPath: Failed to parse address 2: %s\n",SMPath_GetParseErrorString(&addr2));
	    PS2Driver_WaitForKeyPress("SMPath Pause");
        return;
	}

	if (SMPath_Parse(&addr3, test_address_3))
	{
		SMPath_Debug(&addr3);
	}
	else
	{
		printf("SMPath: Failed to parse address 3: %s\n",SMPath_GetParseErrorString(&addr3));
	    PS2Driver_WaitForKeyPress("SMPath Pause");
        return;
	}
    PS2Driver_WaitForKeyPress("SMPath: Parser test passed");
}