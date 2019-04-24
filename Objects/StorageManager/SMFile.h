#pragma once

#include <stdint.h>
#include <stdbool.h>

struct SMVolume;

struct SMFile
{

};

bool SMFile_Constructor(struct SMFile* self, struct SMVolume* volume, uint32_t first_cluster);