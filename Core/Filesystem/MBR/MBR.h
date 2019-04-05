#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <Filesystem/MBR/MBRRecord.h>

struct ATADriver;
struct ATA_IDEDevice;

struct MBR
{
    bool Debug;
    struct ATADriver* ATA;
    struct ATA_IDEDevice* IDEDevice;
    struct MBRRecord Record;
};

bool MBR_Constructor(struct MBR* self, struct ATADriver* ata, uint8_t ata_device_id);