
#pragma once

#include <Objects/StorageManager/Directory.h>

#define VOLUME_NAME_SIZE 32

struct Volume
{
    char Name[VOLUME_NAME_SIZE];
    struct Directory RootDirectory;
};