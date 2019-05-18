#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "FAT/FatConstants.h"
#include <Objects/Common/LinkedList/LinkedList.h>

#define SM_PATH_MAX_DIRS 16
#define SM_PATH_MAX_DRIVE 3
#define SM_PATH_MAX_VOLUME 3
#define SM_PATH_BUS_NAME_LEN 3
#define SM_PATH_DEVICE_DELIMETER "://"
#define SM_PATH_DELIMETER '/'
#define SM_PATH_DEVICE_BUFFER_LENGTH 8
#define SM_PATH_VOLUME_DELIMETER 'p'
#define SM_PATH_BUS_FDD_STR "FDD"
#define SM_PATH_BUS_FDD_LOWER_STR "fdd"
#define SM_PATH_BUS_ATA_STR "ATA"
#define SM_PATH_BUS_ATA_LOWER_STR "ata"
#define SM_PATH_BUS_RAM_STR "RAM"
#define SM_PATH_BUS_RAM_LOWER_STR "ram"

/**
 * A path to a file should appear in the following format
 * 
 *      <DEVICE>://<Directory/File>/[Directory or File]/...
 *  Legend
 *      <>  - Mandatory
 *      []  - Optional
 *      ... - Recurring
 *      /   - Path Delimeter 
 *      :// - Device Delimeter
 * 
 * 
 * Where Device is one of
 *      * FDD / fdd
 *      * ata / ata
 * 
 * For FD (Floppy Disk)
 *      * 0 - Floppy Drive 0
 *      * 1 - Floppy Drive 1
 * 
 * For ATA (HDD/CD)
 *      * 0 - ATA Drive 0 
 *      * 1 - ATA Drive 1 
 *      * 2 - ATA Drive 2 
 *      * 3 - ATA Drive 3 
 * 
 * For Volume
 *      * v0
 *      * v1
 *      * v2
 *      * v3
 * 
 *  e.g.
 *      * fdd0://SomeDir/SomeFile.txt
 *      * ata2v0://SomeFile.txt
 *      * ata0v3://SomeDir/SomeSubdir/SomeFile.txt
 */

/**
 * @brief Errors produced when parsing a directory string
 */
enum SMPathParseError
{
    SM_PATH_NO_ERROR = 0,
    SM_PATH_ERR_INVALID_DRIVE,
    SM_PATH_ERR_INVALID_VOLUME,
    SM_PATH_ERR_INVALID_PATH_CHAR,
    SM_PATH_ERR_BLANK_NAME,
    SM_PATH_ERR_NAME_TOO_LONG,
    SM_PATH_ERR_NO_ROOT_DELIMETER
};

enum SMPathDriveType
{
    DRIVE_TYPE_ERROR = 0,
    DRIVE_TYPE_FLOPPY,
    DRIVE_TYPE_ATA,
    DRIVE_TYPE_RAMDISK
};

/**
 * @brief Single-use object that describes a file or directory's location in the filesystem. Including
 * disk number, volume number and path.
 */
struct SMPath
{
    bool    Debug;
    enum    SMPathParseError ParseError;

    char    DeviceString [SM_PATH_DEVICE_BUFFER_LENGTH];
    enum    SMPathDriveType DriveType;
    int8_t  DriveIndex;
    int8_t  VolumeIndex;
    uint8_t WalkIndex;
    struct  LinkedList Directories;
};

/**
 * @brief Constructs a SMPath object.
 */
bool SMPath_Constructor(struct SMPath* self);

/**
 * @brief Constructs a SMPath object.
 */
bool SMPath_Destructor(struct SMPath* self);

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