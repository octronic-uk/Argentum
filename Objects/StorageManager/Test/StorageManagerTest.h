#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct StorageManager StorageManager;

bool StorageManagerTest_RunSuite(StorageManager* self);
void StorageManagerTest_DebugData(uint8_t* data, uint32_t size);
