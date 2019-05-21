#include <stdio.h>
#include <stdlib.h>

#include "FILE.h"
#include <Objects/StorageManager/SMFileMode.h>
#include <Objects/StorageManager/SMDirectoryEntry.h>

// TODO - Needs Standard compliance
int fseek(FILE* stream, long int offset, int origin)
{   
    printf("fseek: offset: %d, origin: %d\n", offset, origin);

    uint32_t result = 0;
    if (!stream) return result;

    if (stream->FileMode->Binary)
    {
        /*
            If the stream is open in binary mode, the new position is exactly offset bytes measured 
            from the beginning of the file if origin is SEEK_SET, from the current file position if 
            origin is SEEK_CUR, and from the end of the file if origin is SEEK_END. Binary streams 
            are not required to support SEEK_END, in particular if additional null bytes are output. 
        */
        switch (origin)
        {
            case SEEK_SET:
                result = SMDirectoryEntry_SetFileOffset(stream->DirectoryEntry, offset);
                break;
            case SEEK_CUR:
                result = SMDirectoryEntry_SetFileOffset(stream->DirectoryEntry, stream->DirectoryEntry->FileOffset.ByteOffset + offset);
                break;
            case SEEK_END:
                result = SMDirectoryEntry_SetFileOffset(stream->DirectoryEntry, stream->DirectoryEntry->FatDirectoryEntrySummary.FileSize);
                break;
            default: return 0;
        }
    }
    else
    {
        /*
            If the stream is open in text mode, the only supported values for offset are zero (which 
            works with any origin) and a value returned by an earlier call to ftell on a stream 
            associated with the same file (which only works with origin of SEEK_SET).
        */
        switch (origin)
        {
            case SEEK_SET:
                result = SMDirectoryEntry_SetFileOffset(stream->DirectoryEntry, offset);
                break;
            case SEEK_CUR:
                result = SMDirectoryEntry_SetFileOffset(stream->DirectoryEntry, stream->DirectoryEntry->FileOffset.ByteOffset + offset);
                break;
            case SEEK_END:
                result = SMDirectoryEntry_SetFileOffset(stream->DirectoryEntry, stream->DirectoryEntry->FatDirectoryEntrySummary.FileSize);
                break;
            default: return 0;
        }
    }
    
    return result != offset;
}