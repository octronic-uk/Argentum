#pragma once

#include <stdbool.h>

#define DIRECTORY_NAME_SIZE 32
#define DIRECTORY_PATH_SIZE 128

struct Volume;

struct Directory
{
    struct Volume* Volume;
    struct Directory* Parent;
    char Name[DIRECTORY_NAME_SIZE];
    char Path[DIRECTORY_PATH_SIZE];
};

bool Directory_Constructor(struct Directory* self, struct Directory* parent, struct Volume* volume);
void Directory_Destructor(struct Directory* self);