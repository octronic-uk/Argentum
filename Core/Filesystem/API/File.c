#include <Filesystem/API/File.h>

File* File_Create(char* path)
{

}

File* File_Open(char* path)
{

}

void File_Close(File* file)
{

}

bool File_Delete(char* path)
{

}

uint32_t File_SeekBeginning(File* file) 
{

}

uint32_t File_SeekAbsolute(File* file, uint32_t offset) 
{

}

uint32_t File_SeekEnd(File* file) 
{

}

uint32_t File_GetCurrentOffset(File* file) 
{

}

uint8_t File_Read8(File* file) 
{
    
}

bool File_Write8(File* file, uint8_t data)
{

}

uint16_t File_Read16(File* file) 
{

}

bool File_Write16(File* file, uint16_t data) 
{

}

uint32_t File_Read32(File* file) 
{

}

bool File_Write32(File* file, uint32_t data) 
{

}

bool File_ReadString(File* file, uint8_t* buffer, uint32_t count) 
{

}

bool File_WriteString(File* file, uint8_t* buffer, uint32_t count) 
{

}