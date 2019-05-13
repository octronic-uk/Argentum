
#pragma once

#include <stdbool.h>

/*
    "r"	
        read: Open file for input operations. The file must exist.
    "w"	
        write: Create an empty file for output operations. If a file with the same name already exists, 
        its contents are discarded and the file is treated as a new empty file.
    "a"	
        append: Open file for output at the end of a file. Output operations always write data at 
        the end of the file, expanding it. Repositioning operations (fseek, fsetpos, rewind) are 
        ignored. The file is created if it does not exist.
    "r+"
        read/update: Open a file for update (both for input and output). The file must exist.
    "w+"	
        write/update: Create an empty file and open it for update (both for input and output). If a 
        file with the same name already exists its contents are discarded and the file is treated as
        a new empty file.
    "a+"
        append/update: Open a file for update (both for input and output) with all output operations
        writing data at the end of the file. Repositioning operations (fseek, fsetpos, rewind) affects
        the next input operations, but output operations move the position back to the end of file.
        The file is created if it does not exist.

    With the mode specifiers above the file is open as a text file. In order to open a file as a 
    binary file, a "b" character has to be included in the mode string. This additional "b" character
    can either be appended at the end of the string (thus making the following compound modes: 
    "rb", "wb", "ab", "r+b", "w+b", "a+b") or be inserted between the letter and the "+" sign for 
    the mixed modes ("rb+", "wb+", "ab+").

*/

#define SM_FILEMODE_READ   'r'
#define SM_FILEMODE_WRITE  'w'
#define SM_FILEMODE_APPEND 'a'
#define SM_FILEMODE_BINARY 'b'
#define SM_FILEMODE_UPDATE '+'

struct SMFileMode
{
    bool Append;
    bool Binary;
    bool Read;
    bool Update;
    bool Write;
};
typedef struct SMFileMode SMFileMode; 

bool SMFileMode_Constructor(SMFileMode* self, const char* mode);