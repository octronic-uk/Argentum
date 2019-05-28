#include "FILE.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <Objects/StorageManager/SMFileMode.h>
#include <Objects/StorageManager/SMDirectoryEntry.h>
#include <Objects/Kernel/Kernel.h>

extern Kernel _Kernel;

bool FILE_Constructor(FILE* self, const char* path, const char* mode)
{
    printf("FILE: Constructor\n");
    memset(self, 0, sizeof(FILE));
    strcpy(self->FilePath,path);
    self->FileMode = (SMFileMode*)malloc(sizeof(SMFileMode));

    if (!SMFileMode_Constructor(self->FileMode, mode))
    {
        free(self->FileMode);
        return false;
    }
    self->DirectoryEntry = malloc(sizeof(SMDirectoryEntry));
   
    return true;
}

void FILE_Destructor(FILE* self)
{
    free(self->FileMode);
    free(self->DirectoryEntry);
}