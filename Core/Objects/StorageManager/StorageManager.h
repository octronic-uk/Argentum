#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <Objects/Structures/LinkedList.h>
#include <Objects/StorageManager/Drive.h>

struct Kernel;

struct StorageManager
{
    struct Kernel* Kernel;
    struct LinkedList Drives;
};

bool StorageManager_Constructor(struct StorageManager* self, struct Kernel* kernel);
bool StorageManager_Destructor(struct StorageManager* self);