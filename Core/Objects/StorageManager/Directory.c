#include <Objects/StorageManager/Directory.h>

#include <string.h>

bool Directory_Constructor(struct Directory* self, struct Directory* parent, struct Volume* volume)
{
    self->Parent = parent;
    self->Volume = volume;
    memset(self->Name,0,DIRECTORY_NAME_SIZE);
    memset(self->Path,0,DIRECTORY_PATH_SIZE);
    return true;
}

void Directory_Destructor(struct Directory* directory)
{

}