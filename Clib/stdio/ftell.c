#include <stdio.h>
#include "FILE.h"
#include <Objects/HardwareManager/StorageManager/SMDirectoryEntry.h>

long int ftell(FILE* stream)
{
    printf("ftell:\n");
    return stream->DirectoryEntry->FileOffset.ByteOffset;
}