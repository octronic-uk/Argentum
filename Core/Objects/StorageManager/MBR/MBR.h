#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "MBRRecord.h"

struct ATADriver;
struct ATA_IDEDevice;

/**
 * @brief Contains objects required to work with the MBR as well as the MBRRecord itself
 */
struct MBR
{
    bool Debug;
    struct ATADriver* ATA;
    struct ATA_IDEDevice* IDEDevice;
    struct MBRRecord Record;
};

/**
 * @brief Uses the given parameters to construct an MBR object and read the MBR Record from disk
*/
bool MBR_Constructor(struct MBR* self, struct ATADriver* ata, uint8_t ata_device_id);