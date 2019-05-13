#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "MBRRecord.h"

typedef struct ATADriver ATADriver;
typedef struct ATA_IDEDevice ATA_IDEDevice;

/**
 * @brief Contains objects required to work with the MBR as well as the MBRRecord itself
 */
struct MBR
{
    bool Debug;
    ATADriver* ATA;
    ATA_IDEDevice* IDEDevice;
    MBRRecord Record;
};
typedef struct MBR MBR;

/**
 * @brief Uses the given parameters to construct an MBR object and read the MBR Record from disk
*/
bool MBR_Constructor(MBR* self, ATADriver* ata, uint8_t ata_device_id);