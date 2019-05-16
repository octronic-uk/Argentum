#include "FILE.h"

#include <string.h>
#include <stdio.h>
#include <Objects/StorageManager/SMFileMode.h>
#include <Objects/StorageManager/SMDirectoryEntry.h>
#include <Objects/Kernel/Kernel.h>

extern Kernel _Kernel;

bool FILE_Constructor(FILE* self, const char* path, const char* mode)
{
    printf("FILE: Constructor\n");
    memset(self, 0, sizeof(FILE));
    strcpy(self->FilePath,path);
    self->FileMode = (SMFileMode*)MemoryDriver_Allocate(&_Kernel.Memory, sizeof(SMFileMode));

    if (!SMFileMode_Constructor(self->FileMode, mode))
    {
        MemoryDriver_Free(&_Kernel.Memory, self->FileMode);
        return false;
    }
    self->DirectoryEntry = MemoryDriver_Allocate(&_Kernel.Memory, sizeof(SMDirectoryEntry));
   
    return true;
}

void FILE_Destructor(FILE* self)
{
    MemoryDriver_Free(&_Kernel.Memory, self->FileMode);
    MemoryDriver_Free(&_Kernel.Memory, self->DirectoryEntry);
}