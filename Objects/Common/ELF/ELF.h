#pragma once

#include <stdbool.h>
#include "Types.h"

bool ELF_CheckFile(ELF32_Header *hdr);
bool ELF_CheckSupported(ELF32_Header *hdr);
void *ELF_LoadRel(ELF32_Header *hdr);
void *ELF_LoadFile(void *file);
int ELF_LoadStage1(ELF32_Header *hdr);
ELF32_SectionHeader* ELF_GetSectionHeader(ELF32_Header *hdr);
ELF32_SectionHeader* ELF_Section(ELF32_Header *hdr, int idx);
char* ELFStringTable(ELF32_Header *hdr);
char* ELF_LookupString(ELF32_Header *hdr, int offset);
int ELF_GetSymbolValue(ELF32_Header *hdr, int table, uint32_t idx);
int ELF_LoadStage2(ELF32_Header *hdr);
int ELF_DoReloc(ELF32_Header *hdr, Elf32_Rel *rel, ELF32_SectionHeader *reltab);
void* ELF_LookupSymbol(const char *name);