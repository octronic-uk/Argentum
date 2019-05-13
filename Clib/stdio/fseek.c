#include <stdio.h>

#include "FILE.h"
#include <Objects/StorageManager/SMDirectoryEntry.h>

// TODO - Needs Standard compliance
int fseek(FILE* stream, long int offset, int origin)
{
    uint32_t result = SMDirectoryEntry_SetFileOffset(stream->DirectoryEntry, offset);
    return result != offset;
}