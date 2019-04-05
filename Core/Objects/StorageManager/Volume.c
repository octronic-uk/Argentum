#include <Objects/StorageManager/Volume.h>

#include <string.h>

void Volume_Constructor(struct Volume* self)
{
    memset(self->Name,0,VOLUME_NAME_SIZE);
    Directory_Constructor(&self->RootDirectory, 0, self);
}