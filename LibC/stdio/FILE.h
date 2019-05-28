#pragma once

#include <stdbool.h>
typedef struct SMFileMode SMFileMode;
typedef struct SMDirectoryEntry SMDirectoryEntry;

struct FILE
{
    char FilePath[128];
    SMFileMode* FileMode;
    SMDirectoryEntry* DirectoryEntry;
};
typedef struct FILE FILE;


bool FILE_Constructor(FILE* self, const char* path, const char* mode);
void FILE_Destructor(FILE* self);