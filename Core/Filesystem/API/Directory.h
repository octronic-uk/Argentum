#pragma once

#define DIRECTORY_PATH_SIZE 128

struct Directory
{
    struct Directory* Parent;
    char Path[DIRECTORY_PATH_SIZE];
};