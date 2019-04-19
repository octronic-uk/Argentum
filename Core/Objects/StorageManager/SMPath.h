#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "FAT16/FatConstants.h"
#define SM_PATH_MAX_DIRS 16
#define SM_PATH_MAX_DRIVE 3
#define SM_PATH_MAX_VOLUME 3
#define SM_PATH_DELIMETER '/'
#define SM_PATH_BUS_BUFFER_LENGTH 8

/**
 * A path to a file should appear in the following format
 * 
 *      / <BUS> / <DEVICE> / [VOLUME] / <Directory/File> / [Directory or File] / ...
 *  Legend
 *      <>  - Mandatory
 *      []  - Optional
 *      ... - Recurring
 *      /   - Path Delimeter 
 * 
 * 
 * Where BUS is one of
 *      * FD
 *      * IDE
 * 
 * DEVICE
 * 
 * For FD (Floppy Disk)
 *      * 0 - Floppy Drive 0
 *      * 1 - Floppy Drive 1
 * 
 * For IDE (HDD/CD)
 *      * 0 - ATA/ATAPI Disk 0 
 *      * 1 - ATA/ATAPI Disk 1 
 *      * 2 - ATA/ATAPI Disk 2 
 *      * 3 - ATA/ATAPI Disk 3 
 */

/**
 * @brief Errors produced when parsing a directory string
 */
enum SMPathParseError
{
    SM_PATH_NO_ERROR = 0,
    SM_PATH_ERR_NO_BEGIN_CHAR,
    SM_PATH_ERR_INVALID_BUS,
    SM_PATH_ERR_INVALID_DRIVE,
    SM_PATH_ERR_INVALID_VOLUME,
    SM_PATH_ERR_INVALID_PATH_CHAR,
    SM_PATH_ERR_BLANK_NAME,
    SM_PATH_ERR_NAME_TOO_LONG,
    SM_PATH_ERR_NO_ROOT_DELIMETER
};


/**
 * @brief Single-use object that describes a file or directory's location in the filesystem. Including
 * disk number, volume number and path.
 */
struct SMPath
{
    bool    Debug;
    enum    SMPathParseError ParseError;
    char    Bus[SM_PATH_BUS_BUFFER_LENGTH];
    int8_t  DriveIndex;
    int8_t  VolumeIndex;
    uint8_t WalkIndex;
    char    Directories[SM_PATH_MAX_DIRS][FAT_LFN_NAME_SIZE];
};

/**
 * @brief Constructs a SMPath object.
 */
bool SMPath_Constructor(struct SMPath* self);

/**
 * Constructs a SMPath object and parses the provided path.
 */
bool SMPath_ConstructAndParse(struct SMPath* self, const char* path);

/**
 * @brief Parse the provided string into a SMPath object. Populates ParseError if an error occurred.
 */
bool SMPath_Parse(struct SMPath* self, const char* address_string);

/**
 * @brief True if the character is valid within a path.
 */
bool SMPath_ValidPathChar(char c);

/**
 * @brief Returns a human-readable error string for the SMParseError that occurred.
 */
const char* SMPath_GetParseErrorString(struct SMPath*);

/**
 * @brief Prints debug information about the SMPath object
 */
void SMPath_Debug(struct SMPath* self);


void SMPath_TestParser();