#include <stdio.h>
#include "FILE.h"
#include <Objects/StorageManager/SMDirectoryEntry.h>

uint32_t fread(void* ptr, uint32_t size, uint32_t count, FILE* stream)
{
    if (!ptr) return 0;
    if (!size) return 0;
    if (!count) return 0;

    uint32_t bytes_read = SMDirectoryEntry_Read(stream->DirectoryEntry,size*count,ptr);
}