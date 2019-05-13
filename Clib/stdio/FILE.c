#include "FILE.h"

#include <string.h>
#include <Objects/StorageManager/SMFileMode.h>
#include <Objects/StorageManager/SMDirectoryEntry.h>
#include <Objects/Kernel/Kernel.h>

extern Kernel _Kernel;

bool FILE_Constructor(FILE* self, const char* path, const char* mode)
{
    memset(self, 0, sizeof(FILE));
    strcpy(self->FilePath,path);
    self->FileMode = MemoryDriver_Allocate(&_Kernel.Memory, sizeof(SMFileMode));
    if (!SMFileMode_Constructor(self->FileMode, mode))
    {
        MemoryDriver_Free(&_Kernel.Memory, self->FileMode);
        return false;
    }
    self->DirectoryEntry = MemoryDriver_Allocate(&_Kernel.Memory, sizeof(SMDirectoryEntry));
    if (!StorageManager_Open(&_Kernel.StorageManager,self->DirectoryEntry, path))
    {
        MemoryDriver_Free(&_Kernel.Memory, self->DirectoryEntry);
        return false;
    }
    return true;
}

void FILE_Destructor(FILE* self)
{
    MemoryDriver_Free(&_Kernel.Memory, self->FileMode);
    MemoryDriver_Free(&_Kernel.Memory, self->DirectoryEntry);
}