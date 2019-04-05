#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <Objects/Structures/LinkedList.h>

struct Memory;

struct Drive
{
    struct Memory* Memory;
    struct LinkedList Volumes;
};

bool Drive_Constructor(struct Drive* self, struct Memory* memory);
bool Drive_Destructor(struct Drive* self);