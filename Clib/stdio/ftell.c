#include <stdio.h>
#include "FILE.h"
#include <Objects/StorageManager/SMDirectoryEntry.h>

long int ftell(FILE* stream)
{
    return stream->DirectoryEntry->FileOffset.ByteOffset;
}