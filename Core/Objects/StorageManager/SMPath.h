#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "FAT/FatConstants.h"
#define SM_PATH_MAX_DIRS 16
#define SM_PATH_MAX_DRIVE 3
#define SM_PATH_MAX_VOLUME 3
#define SM_PATH_PATH_DELIMETER '/'
// Errors
enum SMPathError
{
    SM_PATH_NO_ERROR = 0,
    SM_PATH_ERR_NO_BEGIN_CHAR,
    SM_PATH_ERR_INVALID_DRIVE,
    SM_PATH_ERR_INVALID_VOLUME,
    SM_PATH_ERR_INVALID_PATH_CHAR,
    SM_PATH_ERR_BLANK_NAME,
    SM_PATH_ERR_NAME_TOO_LONG,
    SM_PATH_ERR_NO_ROOT_DELIMETER
};


struct SMPath
{
    bool Debug;
    enum SMPathError Error;
    int8_t DriveIndex;
    int8_t VolumeIndex;
    uint8_t WalkIndex;
    char Directories[SM_PATH_MAX_DIRS][FAT_LFN_NAME_SIZE];
};

bool SMPath_Constructor(struct SMPath* self);
bool SMPath_ConstructAndParse(struct SMPath* self, const char* path);
bool SMPath_Parse(struct SMPath* self, const char* address_string);
bool SMPath_ValidPathChar(char c);
const char* SMPath_GetErrorString(struct SMPath*);
void SMPath_Debug(struct SMPath* self);


void SMPath_TestParser();